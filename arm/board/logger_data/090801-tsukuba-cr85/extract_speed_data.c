
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

int
main (int argc, char *argv[])
{
  FILE *f, *lap_file;
  uint32_t r;
  size_t sz;
  char lap_file_name[64];

  if (argc < 2)
    {
      fprintf (stderr, "specify filename\n");
      return 1;
    }

  f = fopen (argv[1], "r");
  assert (f);
  sprintf (lap_file_name, "%s.raw_speed", argv[1]);

  lap_file = fopen (lap_file_name, "w");
  assert (lap_file);
  while ((sz = fread (&r, 1, 4, f)) != 0)
    {
      if (r == 0xffffffff)
	{
	  fprintf (stderr, "last marker found.\n");
	  break;
	}
      //      fprintf (stderr, "%08x\n", r);

      if ((r & 0x10000000) && !(r & 0x20000000))
	{
	  fwrite (&r, 1, 4, lap_file);
	}
    }

  fclose (lap_file);
  fclose (f);

  return 0;
}
