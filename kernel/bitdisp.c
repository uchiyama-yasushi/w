
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

#include <sys/system.h>
#include <sys/console.h>

#define	BITDISP(a, prf)					\
({							\
  typeof (a) _j, _j1;					\
  int32_t _i, _n;						\
  _n = sizeof (typeof(a)) * NBBY - 1;			\
  _j1 = 1 << _n;					\
  for (_j = _j1; _j > 0; _j >>=1)			\
    {							\
      prf ("%c", a & _j ? '|' : '.');			\
    }							\
  for (_j = _j1, _i = _n; _j > 0; _j >>=1, _i--)	\
    {							\
      if (!(_i > 0 || _i < 0) && (a & _j))		\
	{						\
	  prf (" %ld", _i);				\
	}						\
    }							\
  prf (" [0x%lx] %ld", (uint32_t)a, (int32_t)a);	\
  prf ("\n");						\
 })

void
bitdisp32 (uint32_t a)
{

  BITDISP (a, printf);
}

void
bitdisp16 (uint16_t a)
{

  BITDISP (a, printf);
}

void
bitdisp8 (uint8_t a)
{

  BITDISP (a, printf);
}

void
ibitdisp32 (uint32_t a)
{

  BITDISP (a, iprintf);
}

void
ibitdisp16 (uint16_t a)
{

  BITDISP (a, iprintf);
}

void
ibitdisp8 (uint8_t a)
{

  BITDISP (a, iprintf);
}
