#include <sys/types.h>
#include <stdio.h>
#include <math.h>

float lap[] =
  {
    32.487,
    35.261,
    48.804,
    27.893,
    31.087,
    11.789,
    25.420,
    30.879,
    11.683,
    25.500,
    30.719,
    11.731,
    25.254,
    30.849,
    11.732,
    25.282,
    30.786,
    11.639,
    25.254,
    30.920,
    11.633,
    25.332,
    30.846,
    11.811,
    25.255,
    30.616,
    11.543,
    25.282,
    30.679,
    11.906,
    25.290,
    30.608,
    11.659,
    25.218,
    30.604,
    11.696,
    25.085,
    30.916,
    11.625,
    25.121,
    30.397,
    11.508,
    25.433,
    30.635,
    11.541,
    25.080,
    30.588,
    11.490,
    31.331,
    41.782,
    17.544,
  };

int
main (int argc, char *argv[])
{
  int n = sizeof lap / sizeof lap[0];
  int i;
  float total = 0.;
  float t0 = 27.7076894;
  float t[n];
  FILE *file = fopen ("plap_cooked.dat", "w");

  for (i = 0; i < n; i++)
    {
      total += lap[i];
      t[i] = total / 60.;
    }
  float offset = total / 60. - t0;

  uint32_t r;
  for (i = 0; i < n; i++)
    {
      r = 0x20000000 | ((uint32_t)((t[i] - offset - .034) * 60000.) + 0x69ca);

      fwrite (&r, 1, 4, file);
      printf ("%f %08x %f\n", t[i] - offset, r,
	      (r & 0x0fffffff)/1000./60.);

    }
  r = 0xffffffff;
  fwrite (&r, 1, 4, file);
  fclose (file);

  return 0;
}
