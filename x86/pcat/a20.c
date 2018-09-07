
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
#include <sys/types.h>
#include <sys/delay.h>

#include <x86/io.h>
#include <pcat/a20.h>
#include <pcat/bios.h>
#include <dev/pckbc.h>

bool
a20_enable (bool enable)
{
  //  uint8_t r;

  if (!a20_test (enable))
    {
      call16 (bios_int15, enable ? 0x2401 : 0x2402, 0);
      mdelay (1);
      if (!a20_test (enable))
	{
#if 0
	  r = pckbc_outport_read ();
	  if (enable)
	    r |= 0x2;
	  else
	    r &= ~0x2;
	  pckbc_outport_write (r);
#else
	  pckbc_outport_write (enable ? 0x03 : 0x01);
#endif
	  mdelay (1);
	}
    }

  return a20_test (enable);
}

bool
a20_test (bool enable)
{
  uint32_t m0, m1;
  bool enabled;
#define	TEST_ADDR	0x400

  m0 = *(volatile uint32_t *)TEST_ADDR;
  *(volatile uint32_t *)((1 << 20) + TEST_ADDR) = m0 ^ 0xa5a5a5a5;
  //  __asm volatile ("wbinvd");

  m1 = *(volatile uint32_t *)TEST_ADDR;
  // restore.
  *(volatile uint32_t *)TEST_ADDR = m0;

  enabled = m1 == m0;

  return enable ? enabled : !enabled;
}
