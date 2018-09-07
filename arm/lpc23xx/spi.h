
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

// Serial Peripheral Interface module.
#ifndef _LPC23XX_SPI_H_
#define	_LPC23XX_SPI_H_
// SPI Control Register
#define	S0SPCR	((volatile uint32_t *)0xe0020000)
#define	 CR_BIT_ENABLE	0x4	// ->BITS
#define	 CR_CPHA	0x8	// Clock Phase
#define	 CR_CPOL	0x10	// Clock Polarity (1: Active low)
#define	 CR_MSTR	0x20	// Master mode select (1: Master mode)
#define	 CR_LSBF	0x40	// LSB First
#define	 CR_SPIE	0x80	// SPI Interrupt Enable
#define	 CR_BITS_8	0x800
#define	 CR_BITS_9	0x900
#define	 CR_BITS_10	0xa00
#define	 CR_BITS_11	0xb00
#define	 CR_BITS_12	0xc00
#define	 CR_BITS_13	0xd00
#define	 CR_BITS_14	0xe00
#define	 CR_BITS_15	0xf00
#define	 CR_BITS_16	0x000
#define	CR_VALID_BITS	0xffc

// SPI Status Register
#define	S0SPSR	((volatile uint32_t *)0xe0020004)	//RO
#define	 SR_ABRT	0x8	// Slave abort.
#define	 SR_MODF	0x10	// Mode fault.
#define	 SR_ROVR	0x20	// Read overrrun
#define	 SR_WCOL	0x40	// Write collision
#define	 SR_SPIF	0x80	// Transfer complete.
#define	 SR_ERRBITS	(SR_ABRT | SR_MODF | SR_ROVR | SR_WCOL)


// SPI Data Register
#define	S0SPDR	((volatile uint32_t *)0xe0020008)

// SPI Clock Counter Register
#define	S0SPCCR	((volatile uint32_t *)0xe002000c)
// In master mode, this register must be an even number greather than or equal 8
// SCK rate = PCLK_SPI / SPCCR0
#define	CCR_COUNTER_MASK	0xff

// SPI Interrupt Flag.
#define	S0SPINT	((volatile uint32_t *)0xe002001c)
#define	INT_SPI		0x1

#ifdef SPI_VERIFY
//VERIFICATION ONLY REGISTER
// SPI Test Control Register
#define	SPTCR	((volatile uint32_t *)0xe0020010)
#define	 TCR_TEST_MASK	0xfe

// SPI Test Status Register
#define	SPTSR	((volatile uint32_t *)0xe0020010)
#endif
__BEGIN_DECLS
void spi_init (void);
void spi_fini (void);
void spi_putc (uint8_t);
__END_DECLS
#endif
