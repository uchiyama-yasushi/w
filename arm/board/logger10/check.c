
/*-
 * Copyright (c) 2009-2010 UCHIYAMA Yasushi.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//#define	DEBUG
#define	ENABLE_ACCELEROMETER
#define	ENABLE_DISTANCEMETER

typedef int bool;
#define	TRUE	1
#define	FALSE	0

#define	STANDALONE
#include "data.h"

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	fprintf(stderr, "%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))

int NMAGNET = 3;	// default value.
bool label_speed_disable;
bool label_lap_disable;

struct data_process;

struct data_process
{
  uint32_t type;
  bool (*func)(struct data_process *, uint32_t);
  int trailers;
  FILE *file;
  const char *name;
};

bool intr_speed (struct data_process *, uint32_t);
bool intr_lap (struct data_process *, uint32_t);
bool process_gps (struct data_process *, uint32_t);
bool process_accelerometer (struct data_process *, uint32_t);
bool process_distancemeter (struct data_process *, uint32_t);

struct data_process intr_process[] =
  {
    { DATA_TYPE_INTR_SPEED, intr_speed, 0, NULL, "speed" },
    { DATA_TYPE_INTR_LAP, intr_lap, 0, NULL, "lap" },
    { DATA_TYPE_INTR_RPM, NULL, 0, NULL, NULL },
    { DATA_TYPE_INTR_MISC, NULL, 0, NULL, NULL },
  };

struct data_process trailer_process[] =
  {
#ifdef ENABLE_ACCELEROMETER
    { DATA_TYPE_ACCELEROMETER, process_accelerometer, 1, NULL, "accel" },
#endif
#ifdef ENABLE_DISTANCEMETER
    { DATA_TYPE_STROKE_F, process_distancemeter, 1, NULL, "stroke" },
    { DATA_TYPE_STROKE_R, NULL, 1, NULL, NULL },
#endif
    { DATA_TYPE_GEAR, NULL, 1, NULL, NULL },
    { DATA_TYPE_THROTTLE, NULL, 1, NULL, NULL },
    { DATA_TYPE_BRAKE_F, NULL, 1, NULL, NULL },
    { DATA_TYPE_BRAKE_R, NULL, 1, NULL, NULL },
#ifdef ENABLE_GPS
    { DATA_TYPE_GPS, process_gps, 3, NULL, "nmea" },
#endif
    { DATA_TYPE_ERROR, NULL, 0, NULL, "log" },
  };

FILE *input_file, *label_file;

// File ops.
bool open_file (struct data_process *, void *);
FILE *open_file_subr (const char *, const char *);
bool close_file (struct data_process *, void *);
bool read_data (uint32_t *);

// misc dispatcher.
bool data_foreach (bool (*)(struct data_process *, void *), void *);

// Time ops.
float time_current (uint32_t);

// Data ops.
bool process_data (uint32_t);
int data_process_main (const char *);
void unpack_accelerometer (uint32_t, uint16_t *, uint16_t *, uint16_t *);

void usage (void);

int
main (int argc, char *argv[])
{
  int i;

  while ((i = getopt (argc, argv, "m:sl")) != -1)
    {
      switch (i)
	{
	case 'm':	// # of LAP counter magnet.
	  if (!optarg)
	    {
	      usage ();
	      return 1;
	    }
	  NMAGNET = atoi (optarg);
	  break;
	case 's':
	  label_speed_disable = TRUE;
	  break;
	case 'l':
	  label_lap_disable = TRUE;
	  break;
	case '?':
	default:
	  usage ();
	}
    }
  argc -= optind;
  argv += optind;

  if (argc < 1)
    {
      fprintf (stderr, "Specify logger data file.\n");
      usage ();
      exit (1);
    }

  return data_process_main (argv[0]);
}

void
usage ()
{

  fprintf (stderr, "-m #\t: # of magnets (default %d)\n", NMAGNET);
  fprintf (stderr, "-s\t: disable min/max speed label\n");
  fprintf (stderr, "-l\t: disable lap label\n");
}

int
data_process_main (const char *filename)
{
  uint32_t r;

  printf ("# of magnet:%d %s\n", NMAGNET, filename);

  if (!(input_file = fopen (filename, "r")))
    {
      perror (filename);
      return 1;
    }

  if (!data_foreach (open_file, (void *)filename))
    {
      perror ("couldn't open output file.");
      goto end;
    }

  if (!(label_file = open_file_subr (filename, "label")))
    {
      perror ("couldn't open label file.");
      goto end;
    }

  // Skip no-meaning data.
#if 0
  do
    {
      if (!read_data (&r))
	{
	  fprintf (stderr, "no data\n");
	  return 1;
	}
    }
  while (process_data (r) != DATA_TYPE_INTR_SPEED);
  time_set_origin (DATA_TIME (r));
#endif

  do
    {
      if (!read_data (&r))
	{
	  fprintf (stderr, "data end.\n");
	  break;
	}
    }
  while (process_data (r));

 end:
  data_foreach (close_file, NULL);
  fclose (label_file);
  fclose (input_file);

  return 0;
}

bool
data_foreach (bool (*do_func)(struct data_process *, void *), void *arg)
{
  struct data_process *p;
  int i;

  p = intr_process;
  for (i = 0; i < sizeof intr_process / sizeof (intr_process[0]); i++, p++)
    {
      if (!p->name)
	continue;
      if (!do_func (p, arg))
	return FALSE;
    }

  p = trailer_process;
  for (i = 0; i < sizeof trailer_process / sizeof (trailer_process[0]);	i++, p++)
    {
      if (!p->name)
	continue;
      if (!do_func (p, arg))
	return FALSE;
    }

  return TRUE;
}

bool
open_file (struct data_process *p, void *arg)
{
  const char *basename = (const char *)arg;

  if (!(p->file = open_file_subr (basename, p->name)))
    return FALSE;

  return TRUE;
}

FILE *
open_file_subr (const char *basename, const char *suffix)
{
  char file_name[64];

  sprintf (file_name, "%s.%s", basename, suffix);

  return fopen (file_name, "w");
}

bool
close_file (struct data_process *p, void *arg)
{

  if (p->file)
    fclose (p->file);

  return TRUE;
}

bool
read_data (uint32_t *v)
{
  size_t sz;

  if ((sz = fread (v, 1, sizeof (uint32_t), input_file) == 0))
    return FALSE;
  DPRINTF ("%08x\n", *v);

  return TRUE;
}

float
time_current (uint32_t t)
{
  static uint32_t time_origin = 0xffffffff;

  if (time_origin == 0xffffffff)
    time_origin = t;

  return (t - time_origin) / 1000. / 60.;	// min
}

bool
process_data (uint32_t v)
{
  uint32_t type = DATA_TYPE (v);
  uint32_t time = DATA_TIME (v);
  struct data_process *p;
  uint32_t processed = 0;
  int i;

  if (type == DATA_TYPE_END)
    {
      DPRINTF ("end marker found.\n");
      return 0;
    }
  DPRINTF ("type=%x\n", type);

  if (DATA_TYPE_INTR (v))
    {
      p = intr_process;
      for (i = 0; i < sizeof intr_process / sizeof (intr_process[0]); i++, p++)
	if ((p->type & type) && p->func)
	  {
	    processed |= p->type;
	    if (!p->func (p, time))
	      return 0;
	  }
    }
  else
    {
      p = trailer_process;
      for (i = 0; i < sizeof trailer_process / sizeof (trailer_process[0]);
	   i++, p++)
	if (p->type == type)
	  {
	    processed = p->type;
	    if (p->func)
	      if (!p->func (p, time))
		return 0;
	    break;
	  }
    }

  if (!processed)
    {
      DPRINTF ("bogus data %08x. unknown data type.\n", v);
    }

  return processed;
}

bool
intr_speed (struct data_process *ctx, uint32_t t)
{
  static uint32_t time_previous;
  float current_time = time_current (t);
  static float speed_min = 300.;
  static float speed_max;
  static float ospeed;
  static int flip = 1;
  static float min_time, max_time;

  DPRINTF ("%08x\n", t);

  uint32_t interval = t - time_previous;
  if (interval < 35)	// too short interval. probably this is noise.
    return TRUE;

  float speed = (CIRCUMFERENCE * 36) / interval;
#if 0
  //XXX BAD IDEA XXX
  if (fabs (speed - ospeed) > 10.)
    speed = (speed + ospeed) /2;
#endif

  // Determine max/min speed.
  if (speed < speed_min)
    {
      speed_min = speed;
      min_time = current_time;
    }
  if (speed > speed_max)
    {
      speed_max = speed;
      max_time = current_time;
    }

  if (!label_speed_disable)
    {
      if ((speed_max - speed > 10.) && flip)
	{
	  fprintf (label_file, "set label \"%.0f\" at %f, %f\n",
		   speed_max, max_time, speed_max);
	  speed_min = 300.;
	  speed_max = 0;
	  flip ^= 1;
	}

      if ((speed - speed_min > 10.) && !flip)
	{
	  fprintf (label_file, "set label \"%.0f\" at %f, %f\n",
		   speed_min, min_time, speed_min);
	  speed_min = 300.;
	  speed_max = 0;
	  flip ^= 1;
	}
    }

  fprintf (ctx->file, "%f %f\n", current_time, speed);
  time_previous = t;
  ospeed = speed;

  return TRUE;
}

bool
intr_lap (struct data_process *ctx, uint32_t t)
{
  float current_time = time_current (t);
  static uint32_t olap;
  static uint32_t total_lap;
  static int magnet_count;
  static int label_y = 40;

  uint32_t interval = t - olap;

  DPRINTF ("%08x\n", t);

  fprintf (ctx->file, "%f %d %f\n", current_time, 180,
	   100./ ((float)interval + 1.));
  if (!label_lap_disable)
    fprintf (label_file, "set label \"%.1f\" at %f, %d\n",
	     interval/1000., current_time, label_y += 5);

  total_lap += interval;

  if (++magnet_count == NMAGNET)
    {
      if (!label_lap_disable)
	{
	  fprintf (label_file, "set label \"%.2f\" at %f, 60\n",
		   total_lap / 1000., current_time);
	  fprintf (stdout, "%f\n", total_lap / 1000.);
	}
      total_lap = 0;
      magnet_count = 0;
      label_y = 40;
    }

  olap = t;

  return TRUE;
}

bool
process_accelerometer (struct data_process *ctx, uint32_t t)
{
  uint16_t x, y, z;
  uint32_t r;
  DPRINTF ("%08x\n", t);

  if (!read_data (&r))
    return FALSE;

  unpack_accelerometer (r, &x, &y, &z);
  fprintf (ctx->file, "%f %8d %8d %8d\n", time_current (t), x, y, z);

  return TRUE;
}

void
unpack_accelerometer (uint32_t a, uint16_t *x, uint16_t *y, uint16_t *z)
{

  *x = (a >> 20) & 0x3ff;
  *y = (a >> 10) & 0x3ff;
  *z = a & 0x3ff;
}

bool
process_distancemeter (struct data_process *ctx, uint32_t t)
{
  uint32_t r;
  DPRINTF ("%08x\n", t);

  if (!read_data (&r))
    return FALSE;

  fprintf (ctx->file, "%f %8d\n", time_current (t), r);

  return TRUE;
}


bool
process_gps (struct data_process *ctx, uint32_t t)
{
  uint32_t r[3];//latitude, longitude, altitude
  int i;

  DPRINTF ("%08x\n", t);

  for (i = 0; i < 3; i++)
    if (!read_data (r + i))
      return FALSE;

  fprintf (ctx->file,
	   "$GPGGA,0,%d.%d,N,%d.%d,E,1,04,04.2,%d.%d,M,0039.4,M,000.0,0000*4\n",
	   r[0] / 10000, r[0] - (r[0] / 10000)*10000,
	   r[1] / 10000, r[1] - (r[1] / 10000)*10000,
	   r[2], 0);

  return TRUE;
}
