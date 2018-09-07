
/*-
 * Copyright (c) 2008 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _ITU_H_
#define	_ITU_H_

//////////////////////////////////////////////////////////////////////
// COMMON
//////////////////////////////////////////////////////////////////////

// Timer STarT Register
#define	ITU_TSTR	((volatile uint8_t *)0xfff60)	// R/W 0xe0
#define	ITU_TSTR_STR0	0x01
#define	ITU_TSTR_STR1	0x02
#define	ITU_TSTR_STR2	0x04
#define	ITU_TSTR_STR3	0x08
#define	ITU_TSTR_STR4	0x10

#define	ITU_START(x)	__asm volatile ("bset %0, @0x60:8" :: "i"(x))
#define	ITU_STOP(x)	__asm volatile ("bclr %0, @0x60:8" :: "i"(x))

// Timer SyNChro regsiter
#define	ITU_TSNC	((volatile uint8_t *)0xfff61)	// R/W 0xe0
#define	ITU_TSNC_SYNC0	0x01
#define	ITU_TSNC_SYNC1	0x02
#define	ITU_TSNC_SYNC2	0x04
#define	ITU_TSNC_SYNC3	0x08
#define	ITU_TSNC_SYNC4	0x10

// Timer MoDe Register
#define	ITU_TMDR	((volatile uint8_t *)0xfff62)	// R/W 0x80
#define	ITU_TMDR_PWM0	0x01
#define	ITU_TMDR_PWM1	0x02
#define	ITU_TMDR_PWM2	0x04
#define	ITU_TMDR_PWM3	0x08
#define	ITU_TMDR_PWM4	0x10
#define	ITU_TMDR_FDIR	0x20	/* TSR2 Flag DIRection */
#define	ITU_TMDR_MDF	0x40	/* ch2 Phase counting mode Flag */

// Timer Function Control Register: ch3, ch4 PWM mode
#define	ITU_TFCR	((volatile uint8_t *)0xfff63)	// R/W 0xc0
#define	ITU_TFCR_BFA3	0x01
#define	ITU_TFCR_BFB3	0x02
#define	ITU_TFCR_BFA4	0x04
#define	ITU_TFCR_BFB4	0x08
#define	ITU_TFCR_CMD0	0x10
#define	ITU_TFCR_CMD1	0x20
  /*
    CMD1 CMD0
      0   0	ch3,ch4 normal operation
      0   1	ch3,ch4 normal operation 
      1   0	ch3 and ch4 complementary PWM mode
      1   1	ch3 and ch4 reset synchronized PWM mode
   */

// Timer Output master Enable Register: ch3, ch4 output mode.
#define	ITU_TOER	((volatile uint8_t *)0xfff90)	// R/W 0xff
#define	ITU_TOER_EA3	0x01
#define	ITU_TOER_EA4	0x02
#define	ITU_TOER_EB4	0x04
#define	ITU_TOER_EB3	0x08
#define	ITU_TOER_EXA4	0x10
#define	ITU_TOER_EXB4	0x20

// Timer Output Control Register: ch3, ch4 PWM mode
#define	ITU_TOCR	((volatile uint8_t *)0xfff91)	// R/W 0xff
#define	ITU_TOCR_OLS3	0x01	/* level select 3 */
#define	ITU_TOCR_OLS4	0x02	/* level select 4 */
#define	ITU_TOCR_XTGD	0x10	/* eXternal TriGger Disable */

//////////////////////////////////////////////////////////////////////
// ITU 0, 1, 2
//////////////////////////////////////////////////////////////////////

/* Timer Control Register */
#define	ITU0_TCR	((volatile uint8_t *)0xfff64)	// R/W 0x80
#define	ITU1_TCR	((volatile uint8_t *)0xfff6e)	// R/W 0x80
#define	ITU2_TCR	((volatile uint8_t *)0xfff78)	// R/W 0x80
  // TIMER PRESCALER
#define	ITU__TCR_TPSC0	0x01
#define	ITU__TCR_TPSC1	0x02
#define	ITU__TCR_TPSC2	0x04
  // Internal clock
#define	 ITU__TCR_ICLK1	0				 //25  MHz  0.04usec OVF  2.62msec
#define	 ITU__TCR_ICLK2	ITU__TCR_TPSC0			 //12.5MHz  0.08usec OVF  5.24msec
#define	 ITU__TCR_ICLK4	ITU__TCR_TPSC1			 //6.25MHz  0.16usec OVF 10.49msec
#define	 ITU__TCR_ICLK8	(ITU__TCR_TPSC1 | ITU__TCR_TPSC0)//3.125MHz 0.32usec OVF 20.97msec
  // External clock TCLKA, TCLKB, TCLKC, TCLKD
#define	 ITU__TCR_XCLKA	ITU__TCR_TPSC2
#define	 ITU__TCR_XCLKB	(ITU__TCR_TPSC2 | ITU__TCR_TPSC0)
#define	 ITU__TCR_XCLKC	(ITU__TCR_TPSC2 | ITU__TCR_TPSC1)
#define	 ITU__TCR_XCLKD	(ITU__TCR_TPSC2 | ITU__TCR_TPSC1 | ITU__TCR_TPSC01)
  // CLOCK EDGE (for external clock)
#define	ITU__TCR_CKEG0	0x08
#define	ITU__TCR_CKEG1	0x10
#define	 ITU__TCR_EDGE_FALL	0
#define	 ITU__TCR_EDGE_RISE	ITU__TCR_CKEG0
#define	 ITU__TCR_EDGE_BOTH	(ITU__TCR_CKEG1 | ITU__TCR_CKEG0)
  // COUNTER CLEAR
#define	ITU__TCR_CCLR0	0x20
#define	ITU__TCR_CCLR1	0x40
#define	 ITU__TCR_CNT_NOCLEAR	0
#define	 ITU__TCR_CNT_GRA	ITU__TCR_CCLR0
#define	 ITU__TCR_CNT_GRB	ITU__TCR_CCLR1
#define	 ITU__TCR_CNT_SYNC	(ITU__TCR_CCLR0 | ITU__TCR_CCLR1)

/* Timer I/O control Register */
#define	ITU0_TIOR	((volatile uint8_t *)0xfff65)	// R/W 0x88
#define	ITU1_TIOR	((volatile uint8_t *)0xfff6f)	// R/W 0x88
#define	ITU2_TIOR	((volatile uint8_t *)0xfff79)	// R/W 0x88
  //notyet

/* Timer Interrupt Enable Register */
#define	ITU0_TIER	((volatile uint8_t *)0xfff66)	// R/W 0xf8
#define	ITU1_TIER	((volatile uint8_t *)0xfff70)	// R/W 0xf8
#define	ITU2_TIER	((volatile uint8_t *)0xfff7a)	// R/W 0xf8
#define	ITU__TIER_IMIEA	0x01
#define	ITU__TIER_IMIEB	0x02
#define	ITU__TIER_OVIE	0x04
#define	ITU0_INTR_ENABLE(x)  __asm volatile ("bset %0, @0x66:8" :: "i" (x))
#define	ITU1_INTR_ENABLE(x)  __asm volatile ("bset %0, @0x70:8" :: "i" (x))
#define	ITU2_INTR_ENABLE(x)  __asm volatile ("bset %0, @0x7a:8" :: "i" (x))



/* Timer Status Regster  */
#define	ITU0_TSR	((volatile uint8_t *)0xfff67)	// R/(W) 0xf8 zero write only
#define	ITU1_TSR	((volatile uint8_t *)0xfff71)	// R/(W) 0xf8 zero write only
#define	ITU2_TSR	((volatile uint8_t *)0xfff7b)	// R/(W) 0xf8 zero write only
#define	ITU__TSR_IMFA	0x01
#define	ITU__TSR_IMFB	0x02
#define	ITU__TSR_OVF	0x04

/* Timer CouNTer */
#define	ITU0_TCNT	((volatile uint16_t *)0xfff68)	// R/W 0x0000
#define	ITU1_TCNT	((volatile uint16_t *)0xfff72)	// R/W 0x0000
#define	ITU2_TCNT	((volatile uint16_t *)0xfff7c)	// R/W 0x0000

/* General Register A */
#define	ITU0_GRA	((volatile uint16_t *)0xfff6a)	// R/W 0xffff
#define	ITU1_GRA	((volatile uint16_t *)0xfff74)	// R/W 0xffff
#define	ITU2_GRA	((volatile uint16_t *)0xfff7e)	// R/W 0xffff

/* General Register B */
#define	ITU0_GRB	((volatile uint16_t *)0xfff6c)	// R/W 0xffff
#define	ITU1_GRB	((volatile uint16_t *)0xfff76)	// R/W 0xffff
#define	ITU2_GRB	((volatile uint16_t *)0xfff80)	// R/W 0xffff

//////////////////////////////////////////////////////////////////////
// ITU 3, 4
//////////////////////////////////////////////////////////////////////

#define	ITU3_TCR	((volatile uint8_t *)0xfff82)	// R/W 0x80
#define	ITU4_TCR	((volatile uint8_t *)0xfff92)	// R/W 0x80

#define	ITU3_TIOR	((volatile uint8_t *)0xfff83)	// R/W 0x88
#define	ITU4_TIOR	((volatile uint8_t *)0xfff93)	// R/W 0x88

#define	ITU3_TIER	((volatile uint8_t *)0xfff84)	// R/W 0xf8
#define	ITU4_TIER	((volatile uint8_t *)0xfff94)	// R/W 0xf8

#define	ITU3_TSR	((volatile uint8_t *)0xfff85)	// R/(W) 0xf8 zero write only
#define	ITU4_TSR	((volatile uint8_t *)0xfff95)	// R/(W) 0xf8 zero write only

#define	ITU3_TCNT	((volatile uint16_t *)0xfff86)	// R/W 0x0000
#define	ITU4_TCNT	((volatile uint16_t *)0xfff96)	// R/W 0x0000

#define	ITU3_GRA	((volatile uint16_t *)0xfff88)	// R/W 0xffff
#define	ITU4_GRA	((volatile uint16_t *)0xfff98)	// R/W 0xffff

#define	ITU3_GRB	((volatile uint16_t *)0xfff8a)	// R/W 0xffff
#define	ITU4_GRB	((volatile uint16_t *)0xfff9a)	// R/W 0xffff

#define	ITU3_BRA	((volatile uint16_t *)0xfff8c)	// R/W 0xffff
#define	ITU4_BRA	((volatile uint16_t *)0xfff9c)	// R/W 0xffff

#define	ITU3_BRB	((volatile uint16_t *)0xfff8e)	// R/W 0xffff
#define	ITU4_BRB	((volatile uint16_t *)0xfff9e)	// R/W 0xffff


#endif
