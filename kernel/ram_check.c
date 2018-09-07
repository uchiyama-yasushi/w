
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

#include <sys/system.h>
#include <sys/console.h>
#include <stdlib.h>	//random

#define	CHECK_CHUNK	0x100

bool
check_ram (uint32_t start, uint32_t end, bool verbose)
{
  uint8_t *page;
  int i, chunk, x;
  uint32_t s;

  if (verbose)
    {
      iprintf ("Checking RAM %lx-%lx\n", start, end);
    }
  start = ROUND (start, CHECK_CHUNK);
  chunk = CHECK_CHUNK;

  x  = 0;
  for (s = start; s < end; s += chunk)
    {
      page = (uint8_t *)s;
      x = random ();
      for (i = 0; i < chunk; i += 4)
	*(volatile int32_t *)(page + i) = (x ^ i);
	//	*(volatile int *)(page + i) = x;

      for (i = 0; i < chunk; i += 4)
	if (*(volatile int32_t *)(page + i) != (x ^ i))
	  //	if (*(volatile int *)(page + i) != x)
	  {
	    if (verbose)
	      //	      iprintf ("%x != %x @0x%x\n",*(volatile int *)(page + i), x,
	      //		       (uint32_t)(page + i));

	      iprintf ("%lx != %lx @0x%lx\n",*(volatile int32_t *)(page + i), (int32_t)(x ^ i),
		       (uint32_t)(page + i));
	    goto bad;
	  }

      x = random ();
      for (i = 0; i < chunk; i += 4)
	*(volatile int32_t *)(page + i) = (x ^ i);
      for (i = 0; i < chunk; i += 4)
	if (*(volatile int32_t *)(page + i) != (x ^ i))
	  {
	    if (verbose)
	      iprintf ("%lx != %lx @0x%lx\n",*(volatile int32_t *)(page + i), (int32_t)(x ^ i),
		       (uint32_t)(page + i));
	    goto bad;
	  }
    }
  return TRUE;

 bad:
  return FALSE;
}
