
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
/* H8/300H 3052 */

#ifndef _REG_H_
#define	_REG_H_
// MCU mode.
#define	MDCR		((volatile uint8_t *)0xffff1)
#define	ADDR1M_AREA4_START	0x80000
#define	ADDR1M_AREA5_START	0xa0000
#define	ADDR1M_AREA6_START	0xc0000
#define	ADDR1M_AREA_SIZE	0x20000
#define	SYSCR		((volatile uint8_t *)0xffff2)	// R/W 0x0B
#define	 SYSCR_SSBY	0x80	// Software StanBY
#define	 SYSCR_STS2	0x40	// Stanby Timer Select 2
#define	 SYSCR_STS1	0x20	// Stanby Timer Select 1
#define	 SYSCR_STS0	0x10	// Stanby Timer Select 0
#define	 SYSCR_UE	0x08	// Userbit(UI) Enable
#define	 SYSCR_NMIEG	0x04	// NMI EdGe select
#define	 SYSCR_RAME	0x01	// RAM Enable


// Interrupt Controller.
#include "intc.h"
// Bus Controller.
#include "bsc.h"
// Refresh Controller.
#include "rfshc.h"
// DMA Controller
// notyet
// I/O port.
#include "ioport.h"
// Integrated Timer Unit.
#include "itu.h"
// Programmable Timing Pattern Controller
// notyet
// Watch dog timer
// notyet
// Serial Communication Interface.
#include "sci.h"
// Smart Card Interface
// notyet
// A/D Converter.
// notyet
// D/A Converter.
// notyet
// RAM (SYSCR -> Interrupt Controller)
// ROM (FLMCR1,FLMCR2,EBR1,EBR2,RAMCR)
// Clock Pulse Generater (DIVCR)
// Power Down State (MSTCR)


// MCU dependend.
__BEGIN_DECLS
void mcu_mode (void);
__END_DECLS

#endif

