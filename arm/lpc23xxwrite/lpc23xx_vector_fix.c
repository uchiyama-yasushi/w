
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
#include <stdint.h>
#include <stdio.h>

/*
  LPC23XX: The reserved ARM interrupt vector location (0x00000014)
  should contain the 2's complement of the check-sum of the remaining
  interrupt vectors.
 */
int
lpc23xx_vector_fix (FILE *f)
{
  uint32_t buf[8];
  int32_t checksum;
  size_t sz;
  int i;

  rewind (f);
  if ((sz = fread (buf, 1, sizeof buf, f)) != sizeof buf)
    {
      fprintf (stderr, "%s: couldn't read vector region. %zd\n", __FUNCTION__,
	       sz);
      return 1;
    }

  for (i = 0, checksum = 0; i < 8; i++)
    {
      if (i == 5)
	continue;
      checksum += buf[i];
    }
  checksum = -checksum;

  fseek (f, 20, SEEK_SET);
  fwrite (&checksum, 4, 1, f);

  return 0;
}
