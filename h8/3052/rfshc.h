
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

#ifndef _RFSHC_H_
#define	_RFSHC_H_
// ReFreSH Controler

/* ReFreSH Control Register */
#define	RFSHC_RFSHCR		((volatile uint8_t *)0xfffac) // R/W 0x02
// Refresh CYCle Enable
#define	 RFSHCR_RCYCE		0x01
// Refresh pin Enable
#define	 RFSHCR_RFSHE		0x04
// Address multiplex mode select. M9 or M8# column
#define	 RFSHCR_M9M8		0x08
// Strobe mode select CAS or WE#
#define	 RFSHCR_CASWE		0x10
// DRAM Enable.
#define	 _RFSHCR_DRAME		0x20
// PSRAM Enable.
#define	 _RFSHCR_PSRAME		0x40
#define	 RFSHCR_INTERVAL_TIMER	0
#define	 RFSHCR_DRAM		_RFSHCR_DRAME
#define	 RFSHCR_PSRAM		_RFSHCR_PSRAME
#define	 RFSHCR_RESERVED	(_RFSHCR_DRAME | _RFSHCR_PSRAME)
// Self Refresh MoDe
#define	 RFSHCR_SRFMD		0x80

/* Refresh Timer Control / Status Register */
#define	RFSHC_RTMCSR		((volatile uint8_t *)0xfffad) // R/W 0x07
#define	 _RTMCSR_CKS0		0x08
#define	 _RTMCSR_CKS1		0x10
#define	 _RTMCSR_CKS2		0x20
#define	 RTMCSR_CLK_DISABLE	0
#define	 RTMCSR_CLK_2		_RTMCSR_CKS0
#define	 RTMCSR_CLK_8		_RTMCSR_CKS1
#define	 RTMCSR_CLK_32		(_RTMCSR_CKS0 | _RTMCSR_CKS1)
#define	 RTMCSR_CLK_128		_RTMCSR_CKS2
#define	 RTMCSR_CLK_512		(_RTMCSR_CKS2 | _RTMCSR_CKS0)
#define	 RTMCSR_CLK_2048	(_RTMCSR_CKS2 | _RTMCSR_CKS1)
#define	 RTMCSR_CLK_4096	(_RTMCSR_CKS2 | _RTMCSR_CKS1 | _RTMCSR_CKS0)
/*
  System clock 25MHz
			Interval	Overflow (0xff)
   /2	  12.5MHz	0.08us		20.4usec
   /8	 3.125MHz	0.32us		81.6usec
   /32	   781KHz	1.28us	       326.4usec
   /128    195.3KHz	5.12us		 1.3166msec
   /512	    48.83KHz   20.48us		 5.2224msec
   /2048    12.21KHz   81.92us		20.890msec
   /4096     6.10KHz  163.84us		41.779msec
 */
// Compare Match Interrupt Enable (Interval timer mode only)
#define	 RTMCSR_CMIE		0x40
// Compare Match Flag (RTCNT == RTCOR) 
#define	 RTMCSR_CMF		0x80

/* Refresh Timer CouNTer */
// Auto initialize by RTCNT == RTCOR.
#define	RFSHC_RTCNT		((volatile uint8_t *)0xfffae) // R/W 0x00

/* Refresh Time COnstant Register */
#define	RFSHC_RTCOR		((volatile uint8_t *)0xfffaf) // R/W 0xff

#endif
