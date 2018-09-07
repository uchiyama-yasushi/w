
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

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>
#include <sys/debug.h>

#ifdef PWR_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

void
pwr_init ()
{
}

void
pwr_peripheral_module (enum module_power module, bool on)
{
  int shift = module & 0xf;
  int32_t a = (int32_t)module >> 4;	// Sign extension
  volatile uint8_t *r = (volatile uint8_t *)a;

  cpu_status_t s = intr_suspend ();
  uint8_t stbcr = *r;

  if (on)
    {
      stbcr &= ~(1 << shift);
    }
  else
    {
      stbcr |= (1 << shift);
    }
  *r = stbcr;
  intr_resume (s);

  DPRINTF ("%x:%d o%s\n", r, shift, on ? "n" : "ff");
}

void
pwr_info ()
{
  uint32_t stbcr_addr[] =
    {
      PWR_STBCR2_ADDR,
      PWR_STBCR3_ADDR,
      PWR_STBCR4_ADDR,
      PWR_STBCR5_ADDR,
      PWR_STBCR6_ADDR,
      PWR_STBCR7_ADDR,
      PWR_STBCR8_ADDR,
    };
  size_t i;

  for (i = 0; i < sizeof stbcr_addr / sizeof stbcr_addr[0]; i++)
    {
      printf ("STBCR%d: ", i + 2);
      bitdisp8 (*((volatile uint8_t *)stbcr_addr[i]));
    }
}
