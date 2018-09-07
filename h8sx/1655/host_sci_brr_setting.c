
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


#include <stdio.h>
#include <float.h>
#include <stdlib.h>

int
main (int argc, char *argv[])
{
  float bps_list[] = {9600, 38400, 57600, 115200, 230400, 460800};

  if (argc < 2)
    return 0;

  //  float pclk = 24 * 1000000;
  //  float bps = 38400;
  float pclk = atof (argv[1]) * 1000000;
  int i;
  for (i = 0; i < sizeof bps_list / sizeof (bps_list[0]); i++)
    {
      float bps = bps_list [i];
      int n = pclk / (64 / 2 * bps) - 1;
      float diff = (pclk / (64 / 2 * bps * (n + 1)) -1) * 100.;
      float diff1 = (pclk / (64 / 2 * bps * (n + 1 + 1)) -1) * 100.;
      if (fabsf (diff) > fabsf (diff1))
	{
	  n++;
	  diff = diff1;
	}
      printf ("%3d: %8d->%.3f (%.2f%%)\n", n, (int)bps,
	      pclk / (64 / 2 * bps) - 1,  diff);
    }

  return 0;
}
