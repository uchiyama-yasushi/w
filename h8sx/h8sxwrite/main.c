
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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

// Flash Memory Writer for H8SX
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>	// getopt
#include <stdlib.h>	// atoi
#include <assert.h>

#include <string.h>

#include "h8sxwrite.h"

struct h8sxwrite_arg
{
  int ugen_device;
  const char *mot_file;
};

STATIC void usage (void);
STATIC bool parse_option (int, char *[], struct h8sxwrite_arg *,
			  struct board_config *);
STATIC FILE *open_loadfile (const char *, size_t *);

bool verbose;

int
main (int argc, char *argv[])
{
  // Board specific configuration.
  struct board_config board_config;
  // Chip specific configuration.
  struct flash_config flash_config;
  // User option.
  struct h8sxwrite_arg h8sxwrite_arg;
  int i;
  size_t mot_size;
  uint8_t *mot_buf;
  FILE *loadfile = 0;
  int error = 1;
  size_t sz;
  uint8_t *image_buf = 0;
  ssize_t image_sz;

  memset (&h8sxwrite_arg, 0, sizeof h8sxwrite_arg);
  memset (&board_config, 0, sizeof board_config);
  memset (&flash_config, 0, sizeof flash_config);

  // Parse board specific option and connected device.
  if (!parse_option (argc, argv, &h8sxwrite_arg, &board_config))
    return 1;

  // Open target Motorora S-format file.
  if ((loadfile = open_loadfile (h8sxwrite_arg.mot_file, &mot_size)) == NULL)
      goto error;
  if ((mot_buf = malloc (mot_size)) == 0)
    {
      perror ("Can't allocate MOT file buffer.");
      goto error;
    }
  if ((sz = fread (mot_buf, 1, mot_size, loadfile)) != mot_size)
    {
      perror ("Can't read MOT file.");
      goto error;
    }

  // Inquire flash memory geometory
  if (!h8sx_bootprog_init (h8sxwrite_arg.ugen_device, &board_config,
			   &flash_config))
    goto error;

  // Setup flash memory buffer. XXX multiple area.
  image_sz = flash_config.user_mat[0].max - flash_config.user_mat[0].min + 1;
  fprintf (stderr, "Flash memory size = 0x%ld byte\n", image_sz);

  image_buf = malloc (image_sz);
  assert (image_buf);
  memset (image_buf, 0xff, image_sz);

  // Setup flash memory image.
  if (!srec_to_loadimage (mot_buf, mot_size, image_buf, image_sz))
    goto error;

  // Fill unloaded area 0xff
  for (i = image_sz - 1; i >= 0; i--)
    if (image_buf[i] != 0xff)
      break;
  size_t real_sz = i;
  fprintf (stderr, "write %ld/%ld byte\n", real_sz, image_sz);

  // Write out flash memory.
  if (!h8sx_bootprog_write (&flash_config, image_buf, real_sz))
    goto error;
  error = 0;

 error:
  if (image_buf)
    free (image_buf);
  h8sx_bootprog_fini (&flash_config);

  if (loadfile)
    fclose (loadfile);
  fprintf (stderr, "%s\n", error ? "Failed." : "Success.");

  return error;
}

STATIC void
usage ()
{

  fprintf (stderr, "h8sxwrite [-f input clock(MHz)] [-d ugen #] [-m MD_CLK pin] "
	   "file\n");
  fprintf (stderr, "EXAMPLE: h8sxwrite -f 12 -d 0 -m 0 "
	   "foo.mot\n");
  fprintf (stderr, "Input clock 12MHz, connect device is /dev/ugen0.*, "
	   "MD_CLK pull down\n");
}

STATIC bool
parse_option (int argc, char *argv[], struct h8sxwrite_arg *h8sxwrite,
	      struct board_config *board_config)
{
  int i;

  while ((i = getopt (argc, argv, "d:i:f:m:v")) != -1)
    {
      switch (i)
	{
	case 'd':	// ugen device #.
	  if (!optarg)
	    usage ();
	  h8sxwrite->ugen_device = atoi (optarg);
	  break;
	case 'f':	// Input Clock Frequency
	  if (!optarg)
	    usage ();
	  board_config->input_frequency = atoi (optarg);
	  break;
	case 'm':	// MD_CLK pin setting
	  if (!optarg)
	    usage ();
	  board_config->MD_CLK = atoi (optarg);
	  break;
	case 'v':	// Verbose
	  verbose = TRUE;
	}
    }
  argc -= optind;
  argv += optind;

  if (argc < 1)
    {
      fprintf (stderr, "Specify Motorola S-format file.\n");
      usage ();
      return FALSE;
    }
  h8sxwrite->mot_file = argv[0];

  fprintf (stderr, "Output Device [ugen%d]\nH8SX Input Clock [%dMHz]\n"
	   "MDCLK = %d\n",  h8sxwrite->ugen_device,
	   board_config->input_frequency,
	   board_config->MD_CLK);

  return TRUE;
}

STATIC FILE *
open_loadfile (const char *filename, size_t *sz)
{
  struct stat sb;
  FILE *f = NULL;

  fprintf (stderr, "Send file [%s]", filename);
  if (stat (filename, &sb) == -1)
    {
      perror ("stat");
      return NULL;
    }
  *sz = sb.st_size;

  if ((f = fopen (filename, "r+")) == NULL)
    {
      fprintf (stderr, "Couldn't open input file %s\n", filename);
      return NULL;
    }
  fprintf (stderr, ": %zd byte\n", *sz);

  return f;
}
