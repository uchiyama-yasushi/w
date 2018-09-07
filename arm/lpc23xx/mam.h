
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

#ifndef _LPC23XX_MAM_H_
#define	_LPC23XX_MAM_H_
// LPC2388 memory map.
#define	FLASH_ROM_START		0x00000000
#define	FLASH_ROM_SIZE		0x80000

#define	ONCHIP_SRAM_START	0x40000000
#define	ONCHIP_SRAM_SIZE	0x10000

#define	USB_RAM_START		0x7fd00000
#define	USB_RAM_SIZE		0x4000

#define	ETHER_RAM_START		0x7fe00000
#define	ETHER_RAM_SIZE		0x4000

#define	EXTRAM_BANK0_START	0x80000000
#define	EXTRAM_BANK0_SIZE	0x10000

#define	EXTRAM_BANK1_START	0x81000000
#define	EXTRAM_BANK1_SIZE	0x10000

// Memory Mapping Control Register.
#define	MEMMAP	((volatile uint32_t *)0xe01fc040)
#define	 MAP_BOOTLOADER	0x00	//Flash 0x7fffe000
#define	 MAP_USERFLASH	0x01	//Flash	0x00000000
#define	 MAP_USERRAM	0x02	//Internal RAM 0x40000000
#define	 MAP_USEREXTRAM	0x03	//External RAM

// MAM Memory Acceleration Module.
// MAM Control Register
#define	MAMCR   ((volatile uint32_t *)0xe01fc000)
// mode control
#define	 MAM_DISABLE		0x00	// No prefetch
#define	 MAM_PARTENABLE		0x01
// Partially Enable: When non-sequential access, don't use latched data.
#define	 MAM_ENABLE		0x02

// MAM Timing Register
// # of clocks used for Flash memory fetches.
#define	MAMTIM	((volatile uint32_t *)0xe01fc004)
// fetch cycle timing. reset value: 7
#define	MAM_1CLK	0x01
#define	MAM_2CLK	0x02
#define	MAM_3CLK	0x03
#define	MAM_4CLK	0x04
#define	MAM_5CLK	0x05
#define	MAM_6CLK	0x06
#define	MAM_7CLK	0x07

#endif
