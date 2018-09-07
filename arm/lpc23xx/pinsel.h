
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

#ifndef _LPC23XX_PINSEL_H_
#define	_LPC23XX_PINSEL_H_
// Pin connect block

#define	PINSEL0		((volatile uint32_t *)0xe002c000)
#define	PINSEL1		((volatile uint32_t *)0xe002c004)
#define	PINSEL2		((volatile uint32_t *)0xe002c008)
#define	PINSEL3		((volatile uint32_t *)0xe002c00c)
#define	PINSEL4		((volatile uint32_t *)0xe002c010)
#define	PINSEL5		((volatile uint32_t *)0xe002c014)
#define	PINSEL6		((volatile uint32_t *)0xe002c018)
#define	PINSEL7		((volatile uint32_t *)0xe002c01c)
#define	PINSEL8		((volatile uint32_t *)0xe002c020)
#define	PINSEL9		((volatile uint32_t *)0xe002c024)
#define	PINSEL10	((volatile uint32_t *)0xe002c028)

#define	PINMODE0	((volatile uint32_t *)0xe002c040)
#define	PINMODE1	((volatile uint32_t *)0xe002c044)
#define	PINMODE2	((volatile uint32_t *)0xe002c048)
#define	PINMODE3	((volatile uint32_t *)0xe002c04c)
#define	PINMODE4	((volatile uint32_t *)0xe002c050)
#define	PINMODE5	((volatile uint32_t *)0xe002c054)
#define	PINMODE6	((volatile uint32_t *)0xe002c058)
#define	PINMODE7	((volatile uint32_t *)0xe002c05c)
#define	PINMODE8	((volatile uint32_t *)0xe002c060)
#define	PINMODE9	((volatile uint32_t *)0xe002c064)

enum pinsel
  {
    PIN_FUNC0 = 0,
    PIN_FUNC1 = 1,
    PIN_FUNC2 = 2,
    PIN_FUNC3 = 3,
  };

enum pinmode
  {
    PIN_PULLUP = 0,
    PIN_HIZ = 2,
    PIN_PULLDOWN = 3,
  };
__BEGIN_DECLS
void mcu_pin_select (int, int, int);
void mcu_pin_select2 (int, int, enum pinsel);
void mcu_pin_mode (int, int, enum pinmode);
__END_DECLS
#endif
