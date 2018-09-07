
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

#ifdef CLK_DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

STATIC uint32_t __clk_input_clock;
STATIC int __clk_mode;

void
clk_init (enum clk_mode clk_mode, int clkin/*MHz*/,
	  uint16_t ifc, uint16_t pfc)
{
  __clk_input_clock = clkin;
  __clk_mode = clk_mode;
  uint16_t r = *CLK_FRQCR & (FRQCR_STC | FRQCR_CKOEN2);

  r |= (ifc | pfc);

  *CLK_FRQCR = r;
}

void
clk_info ()
{
  struct
  {
    int div1;
    int pll_mult;
    int ckio_mult;
    int ckio_div;
  }  clk_mode_conf [] =  {
    { 1, 12, 4, 1 },
    { 4, 12, 1, 1 },
    { 1, 8,  4, 1 },
    { 3, 8,  4, 3 },
  }, *conf;
  int pfc_div[] = { 4, 6, 8, 12};
  uint16_t r = *CLK_FRQCR;
  int ifc, pfc;
  conf = clk_mode_conf + __clk_mode;

  printf ("FRQCR=%x\n", r);
  ifc = ((r >> FRQCR_IFC_SHIFT) & FRQCR_IFC_MASK) + 1;
  assert (ifc < 4);
  pfc = ((r >> FRQCR_PFC_SHIFT) & FRQCR_PFC_MASK) - 3;
  assert (pfc >= 0 && pfc <= 6);

  printf ("PLL: x%d, Internal x1/%d, Peripheral x1/%d\n",
	  r & FRQCR_STC ? 12 : 8, ifc, pfc_div[pfc]);

  int pllck = __clk_input_clock / conf->div1;
  int div2ck = pllck * conf->pll_mult;

  printf ("Ick=%dMHz, Pck=%dMHz, Bck=%dMHz\n",
	  div2ck / ifc, div2ck / pfc_div[pfc],
	  __clk_input_clock * conf->ckio_mult/ conf->ckio_div);
}

