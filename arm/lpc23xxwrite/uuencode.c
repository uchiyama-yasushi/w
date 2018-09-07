/*
 * uuencode.c -
 *  Simple uuencode utility
 *  Jim Cameron, 1997
 * Modified for LPC23XX by UCHIYAMA Yasushi, 2009
 */

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define	MAX_LINELEN	45
#define	ENCODE_BYTE(b)	(((b) == 0) ? 0x60 : ((b) + 0x20))

size_t
lpc23xx_uuencode (FILE *infile, long int offset, size_t size, FILE *outfile)
{
  ssize_t linelen;
  int linecnt;
  uint8_t inbuf[MAX_LINELEN];
  uint8_t *inbytep;
  char outbuf[5];
  int i;
  int checksum, checksum_line;
  size_t total_size;

  checksum = 0;
  checksum_line = 0;
  total_size = 0;

  if (fseek (infile, offset, SEEK_SET) == -1)
    {
      perror ("fseeko");
      return 0;
    }

  while (/*CONSTCOND*/1)
    {
      /* Read a line from input file */
      size_t readlen = size > MAX_LINELEN ? MAX_LINELEN : size;
      linelen = fread (inbuf, 1, readlen, infile);
      if (linelen == 0)
	break;

      size -= linelen;
      total_size += linelen;

      for (i = 0; i < linelen; i++)
	checksum += inbuf[i];

      /* Write the line length byte */
      fputc (ENCODE_BYTE (linelen), outfile);

      /* Encode the line */
      for (linecnt = linelen, inbytep = inbuf; linecnt > 0;
	   linecnt -= 3, inbytep += 3)
	{
	  /* Encode 3 bytes from the input buffer */
	  outbuf[0] = ENCODE_BYTE ((inbytep[0] & 0xFC) >> 2);
	  outbuf[1] = ENCODE_BYTE (((inbytep[0] & 0x03) << 4) +
				   ((inbytep[1] & 0xF0) >> 4));
	  outbuf[2] = ENCODE_BYTE (((inbytep[1] & 0x0F) << 2) +
				   ((inbytep[2] & 0xC0) >> 6));
	  outbuf[3] = ENCODE_BYTE (inbytep[2] & 0x3F);
	  outbuf[4] = '\0';

	  /* Write the 4 encoded bytes to the file */
	  fprintf (outfile, "%s", outbuf);
	}

      fprintf (outfile, "\n");
      if (++checksum_line == 20)
	{
	  fprintf (outfile, "%d\n", checksum);
	  checksum_line = checksum = 0;
	}
    }

  if (checksum_line)
    {
      fprintf (outfile, "%d\n", checksum);
    }

  return total_size;
}
