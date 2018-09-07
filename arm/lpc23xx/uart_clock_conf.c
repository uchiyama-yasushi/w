
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

// LPC23xx UART clock configuration tool.

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

struct frac
{
  float f;
  int DivAddVal;
  int MulVal;
} frac [] =
  {
    { 1.000000, 0, 1 },
    { 1.066667, 1, 15 },
    { 1.071429, 1, 14 },
    { 1.076923, 1, 13 },
    { 1.083333, 1, 12 },
    { 1.090909, 1, 11 },
    { 1.100000, 1, 10 },
    { 1.111111, 1, 9 },
    { 1.125000, 1, 8 },
    { 1.133333, 2, 15 },
    { 1.142857, 1, 7 },
    { 1.142857, 2, 14 },
    { 1.153846, 2, 13 },
    { 1.166667, 1, 6 },
    { 1.166667, 2, 12 },
    { 1.181818, 2, 11 },
    { 1.200000, 1, 5 },
    { 1.214286, 3, 14 },
    { 1.222222, 2, 9 },
    { 1.230769, 3, 13 },
    { 1.250000, 1, 4 },
    { 1.266667, 4, 15 },
    { 1.272727, 3, 11 },
    { 1.285714, 2, 7 },
    { 1.285714, 4, 14 },
    { 1.300000, 3, 10 },
    { 1.307692, 4, 13 },
    { 1.333333, 1, 3 },
    { 1.357143, 5, 14 },
    { 1.363636, 4, 11 },
    { 1.375000, 3, 8 },
    { 1.384615, 5, 13 },
    { 1.400000, 2, 5 },
    { 1.416667, 5, 12 },
    { 1.428571, 3, 7 },
    { 1.428571, 6, 14 },
    { 1.444444, 4, 9 },
    { 1.454545, 5, 11 },
    { 1.461538, 6, 13 },
    { 1.466667, 7, 15 },
    { 1.500000, 1, 2 },
    { 1.533333, 8, 15 },
    { 1.538462, 7, 13 },
    { 1.545455, 6, 11 },
    { 1.555556, 5, 9 },
    { 1.571429, 4, 7 },
    { 1.571429, 8, 14 },
    { 1.583333, 7, 12 },
    { 1.600000, 3, 5 },
    { 1.615385, 8, 13 },
    { 1.625000, 5, 8 },
    { 1.636364, 7, 11 },
    { 1.642857, 9, 14 },
    { 1.666667, 10,15 },
    { 1.692308, 9, 13 },
    { 1.700000, 7, 10 },
    { 1.714286, 10,14 },
    { 1.714286, 5, 7 },
    { 1.727273, 8, 11 },
    { 1.733333, 11,15 },
    { 1.750000, 3, 4 },
    { 1.769231, 10,13 },
    { 1.777778, 7, 9 },
    { 1.785714, 11,14 },
    { 1.800000, 12,15 },
    { 1.818182, 9, 11 },
    { 1.833333, 10,12 },
    { 1.833333, 5, 6 },
    { 1.846154, 11,13 },
    { 1.857143, 12,14 },
    { 1.857143, 6, 7 },
    { 1.866667, 13,15 },
    { 1.875000, 7, 8 },
    { 1.888889, 8, 9 },
    { 1.900000, 9, 10 },
    { 1.909091, 10,11 },
    { 1.916667, 11,12 },
    { 1.923077, 12,13 },
    { 1.928571, 13,14 },
    { 1.933333, 14,15 },
  };

float fr_guess[] = { 1.5, 1.4, 1.6, 1.3, 1.7, 1.2, 1.8, 1.1, 1.9 };
int bps_table [] = { 9600, 19200, 38400, 57600, 115200, 230400 };

void calc_parameter (int, int);

int
main (int argc, char *argv[])
{
  int peripheral_clock, i;

  if (argc < 2)
    {
      fprintf (stderr, "Specify peripheral clock frequency (Hz).\n");
      return 1;
    }

  peripheral_clock = atoi (argv[1]);

  for (i = 0; i < sizeof bps_table / sizeof (bps_table[0]); i++)
    calc_parameter (peripheral_clock, bps_table[i]);

  return 0;
}

void
calc_parameter (int pclk, int br)
{
  float PCLK, BR, DLest, FRest;
  int iDLest;
  int i;

  PCLK = (float)pclk;
  BR = (float)br;

  DLest= PCLK / (16. * BR);
  if (DLest == (float)((int)PCLK / (16 * (int)BR)))
    {
      i = 0;
      iDLest = (int)DLest;
    }
  else
    {
      // Determne Fractional Divider.
      for (i = 0; i < sizeof fr_guess / sizeof (fr_guess[0]); i++)
	{
	  FRest = fr_guess[i];
	  iDLest = PCLK / (16 * BR * FRest);
	  FRest = PCLK / (16 * BR * iDLest);
	  if (FRest < 1.9 && FRest > 1.1)
	    break;
	}
      for (i = 0; i < sizeof frac / sizeof (frac[0]); i++)
	{
	  if (frac[i].f > FRest)
	    break;
	}
      // best fit.
      if (FRest - frac[i - 1].f < frac[i].f - FRest)
	i--;
    }

  FRest = frac[i].f;
  int DivAddVal = frac[i].DivAddVal;
  int MulVal = frac[i].MulVal;
  int iDMest = (int)(PCLK / (16 * BR * FRest - (float)iDLest)) / 256.;
  float bps = PCLK / ((16. * (256. * (float)iDMest + (float)iDLest) *
		       (1. + ((float)DivAddVal / (float)MulVal))));

  printf (" { %d, %d, %d | (%d << 4), 0 };"
	  " // PCLK %.4fMHz %.0fbps error %.2f%%\n",
	  iDMest, iDLest, DivAddVal, MulVal,
	  PCLK / 1000000, bps, (1 - BR / bps) * 100.);
}
