#include <sys/types.h>
#include <stdio.h>

uint8_t crc7 (uint8_t *buf, int n);

int
main ()
{
  uint8_t buf[] = { 0x41, 0x00, 0x00, 0x00, 0x00 };
  printf ("%x\n", crc7 (buf, sizeof buf));
}

uint8_t
crc7 (uint8_t *buf, int n)
{
  int i, j;
  uint8_t r, crc;

  for (i = 0, crc = 0; i < n; i++)
    {
      r = buf[i];
      for (j = 0; j < 8; j++, r <<= 1)
	crc = (crc << 1) ^ ((r & 0x80) ^ (crc & 0x80) ? 0x12 : 0);
    }

  return crc;
}
