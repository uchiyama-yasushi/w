
/*-
 * Copyright (c) 2009 UCHIYAMA Yasushi.  All rights reserved.
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
#include <math.h>

#define	LOGGER_INTR_SPEED	0x10000000
#define	LOGGER_INTR_LAP		0x20000000

#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))

int
main (int argc, char *argv[])
{
  FILE *f, *speed_file, *lap_file, *label_file;
  size_t sz;
  uint32_t r, t1, r1, olap;
  uint32_t t0;
  char speed_file_name[64];
  char lap_file_name[64];
  char label_file_name[64];

  if (argc < 2)
    {
      fprintf (stderr, "specify filename\n");
      return 1;
    }

  f = fopen (argv[1], "r");
  assert (f);
  sprintf (speed_file_name, "%s.speed", argv[1]);
  sprintf (lap_file_name, "%s.lap", argv[1]);
  sprintf (label_file_name, "%s.label", argv[1]);

  speed_file = fopen (speed_file_name, "w");
  lap_file = fopen (lap_file_name, "w");
  label_file = fopen (label_file_name, "w");
  assert (speed_file && lap_file);

  t1 = 0;
  r1 = 0;
  if ((sz = fread (&t0, 1, 4, f) == 0))
    {
      fprintf (stderr, "no data\n");
      return 1;
    }
  t0 &= 0x0fffffff;
  olap = t1 = t0;
  float ospeed = 0.;
  float current_time = 0.;
  int magnet_count = 0;
  uint32_t total_lap = 0;
  float speed_min, speed_max;
  float min_time, max_time;
  speed_min = 300.;
  speed_max = 0.;
  int label_y = 10;
  int flip = 1;

  while ((sz = fread (&r, 1, 4, f)) != 0)
    {
      if (r == 0xffffffff)
	{
	  fprintf (stderr, "last marker found.\n");
	  break;
	}
      fprintf (stderr, "%08x\n", r);
      uint32_t rval = r & 0x0fffffff;
      current_time = (rval - t0)/1000./60.;

      if (r & 0x20000000)
	{
#if 1
	  if ((rval - olap) < 10000)	//10sec.
	    {
	      fprintf (stderr, "lap noise %08x\n", r);
	      continue;
	    }
#endif
	  fprintf (lap_file, "%f %d %f\n", current_time, 180,
		   (rval - olap)/1000.);
	  fprintf (label_file, "set label \"%.1f\" at %f, %d\n",
		   (rval - olap)/1000., current_time, label_y += 5);
	  total_lap += (rval - olap);
	  if (++magnet_count == 3)
	    {
	      fprintf (label_file, "set label \"%.2f\" at %f, 60\n",
		       total_lap/1000., current_time);
	      total_lap = 0;
	      magnet_count = 0;
	      label_y = 40;
	    }
	  olap = rval;
	}
      if (r & 0x10000000)
	{
	  r1 = rval - t1;
	  if (r1 < 20)
	    continue;

	  float speed = (188 * 36) / r1;
	  if (fabs (speed - ospeed) > 10.)
	    speed = (speed + ospeed) /2;

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

	  if ((speed_max - speed > 10.) && flip)
	    {
	      fprintf (label_file, "set label \"%.0f\" at %f, %f\n",
		       speed_max, max_time, speed_max);
	      speed_max = 0;
	      speed_min = 300.;
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

	  fprintf (speed_file, "%f %f\n", current_time, speed);
	  t1 = rval;
	  ospeed = speed;
	}
    }

  fclose (speed_file);
  fclose (lap_file);
  fclose (f);

  return 0;
}
