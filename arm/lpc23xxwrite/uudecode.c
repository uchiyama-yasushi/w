
/*
 * uudecode.c -
 *  Simple uudecode utility
 *  Jim Cameron, 1997
 * Modified for LPC23XX by UCHIYAMA Yasushi, 2009
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	LINE_BUF_SIZE	256
#define	DECODE_BYTE(b)	((b == 0x60) ? 0 : b - 0x20)

enum state
  {
    SIZE,
    DATA,
    CHECKSUM,
  }
  state;

int
main (int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
  char linebuf[LINE_BUF_SIZE];
  char *linep = NULL;
  int linelen = 0;
  size_t linecnt = 0;
  uint8_t outbyte[3];
  int checksum, checksum_ref, checksum_line;
  int datasize, decoded;
  int last;
  size_t i;

  checksum = 0;
  checksum_line = 0;
  datasize = 0;
  decoded = 0;
  state = SIZE;

  while (/*CONSTCOND*/1)
    {
      if (fgets (linebuf, LINE_BUF_SIZE, stdin) == NULL)
	{
	  fprintf (stderr, "EOF\n");
	  break;
	}
      switch (state)
	{
	case SIZE:
	  datasize = atoi (linebuf);
	  fprintf (stderr, "%dbyte\n", datasize);
	  state = DATA;
	  break;
	case CHECKSUM:
	  checksum_ref = atoi (linebuf);
	  fprintf (stderr, "checksum %d\n", checksum);
	  if (checksum != checksum_ref)
	    {
	      fprintf (stderr, "checksum %d(real) != %d(reference)\n",
		       checksum, checksum_ref);
	      exit (1);
	    }
	  checksum = 0;
	  checksum_line = 0;
	  if (decoded == datasize)
	    {
	      decoded = 0;
	      state = SIZE;
	    }
	  else
	    {
	      state = DATA;
	    }
	  break;
	case DATA:
	  //	  fprintf (stderr, "%s", linebuf);
	  /* The first byte of the line represents the length of the DECODED
	     line */
	  linelen = DECODE_BYTE (linebuf[0]);
	  if (linelen < 45)
	    last = 1;
	  linep = linebuf + 1;
	  for (linecnt = linelen; linecnt > 0; linecnt -= 3, linep += 4)
	    {
	      /* Check for premature end-of-line */
	      if ((linep[0] == '\0') || (linep[1] == '\0') ||
		  (linep[2] == '\0') || (linep[3] == '\0'))
		{
		  fprintf (stderr, "uudecode: Error in encoded block\n");
		  exit (1);
		}

	      /* Decode the 4-byte block */
	      outbyte[0] = DECODE_BYTE (linep[0]);
	      outbyte[1] = DECODE_BYTE (linep[1]);
	      outbyte[0] <<= 2;
	      outbyte[0] |= (outbyte[1] >> 4) & 0x03;
	      outbyte[1] <<= 4;
	      outbyte[2] = DECODE_BYTE (linep[2]);
	      outbyte[1] |= (outbyte[2] >> 2) & 0x0F;
	      outbyte[2] <<= 6;
	      outbyte[2] |= DECODE_BYTE (linep[3]) & 0x3F;

	      /* Write the decoded bytes to the output file */
	      if (linecnt > 3)
		{
		  if (fwrite (outbyte, 1, 3, stdout) != 3)
		    {
		      fprintf (stderr, "uudecode: Error writing to output file\n");
		      exit (1);
		    }
		  for (i = 0; i < 3; i++)
		    checksum += outbyte[i];
		  decoded += 3;
		}
	      else
		{
		  if (fwrite (outbyte, 1, linecnt, stdout) != linecnt)
		    {
		      fprintf (stderr, "uudecode: Error writing to output file\n");
		      exit (1);
		    }
		  for (i = 0; i < linecnt; i++)
		    checksum += outbyte[i];
		  decoded += linecnt;
		  linecnt = 3;
		}
	    }
	  if (decoded == datasize || ++checksum_line == 20)
	    state = CHECKSUM;

	  break;
	}
    }

  return 0;
}
