
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

// H8SX MCU registers.
#ifndef _H8SX_1655_CLOCK_H_
#define	_H8SX_1655_CLOCK_H_

/*
 H8SX/1655		min	max (MHz)
 System Clock		8	50	(CPU,DMAC,EXDMAC,DTC)
 Peripheral Clock	8	35
 External Clock		8	50
*/

// System ClocK Control Register
#define	SCKCR		((volatile uint16_t *)0xfffdc4)
#define	SCKCR_CK_MASK	0x7

#define	SCKCR_BCK_SHIFT	0
#define	SCKCR_BCK0	(1 << 0)
#define	SCKCR_BCK1	(1 << 1)
#define	SCKCR_BCK2	(1 << 2)
#define	SCKCR_BCK(x)	(((x) >> SCKCR_BCK_SHIFT) & SCKCR_CK_MASK)

#define	SCKCR_PCK_SHIFT	4
#define	SCKCR_PCK0	(1 << 4)
#define	SCKCR_PCK1	(1 << 5)
#define	SCKCR_PCK2	(1 << 6)
#define	SCKCR_PCK(x)	(((x) >> SCKCR_PCK_SHIFT) & SCKCR_CK_MASK)

#define	SCKCR_ICK_SHIFT	8
#define	SCKCR_ICK0	(1 << 8)
#define	SCKCR_ICK1	(1 << 9)
#define	SCKCR_ICK2	(1 << 10)
#define	SCKCR_ICK(x)	(((x) >> SCKCR_ICK_SHIFT) & SCKCR_CK_MASK)

#define	SCKCR_PSTOP1	(1 << 15)

__BEGIN_DECLS
// Board specific clock information.
enum clock_select
  {
    CLK_x4 = 0,
    CLK_x2 = 1,
    CLK_x1 = 2,
    CLK_x0_5 = 3,
  };

struct clock_config
{
  uint32_t input_clock;		// Hz
  bool MDCLK;			// MDCLK pin setting.
  // Frequency setting.
  enum clock_select ick;	// CPU clock select
  enum clock_select pck;	// Peripheral clock select.
  enum clock_select bck;	// External bus clock select.
  uint32_t ick_hz;		// CPU clock (Hz)
  uint32_t pck_hz;		// Peripheral clock (Hz)
  uint32_t bck_hz;		// External bus clock (Hz)
};

extern struct clock_config __clock;

bool clock_init (struct clock_config *);
void clock_info (void);

static inline uint32_t
clock_cpu ()
{
  return __clock.ick_hz;
}

static inline uint32_t
clock_peripheral ()
{
  return __clock.pck_hz;
}

static inline uint32_t
clock_external_bus ()
{
  return __clock.bck_hz;
}

__END_DECLS
#endif
