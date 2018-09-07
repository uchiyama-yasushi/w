
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
#ifndef _H8SX_MCU_H_
#define	_H8SX_MCU_H_
// MCU
// MoDe Control Register
#define	MDCR		((volatile uint16_t *)0xfffdc0)
#define	MDCR_MDS0	0x0100	// MD0 pin
#define	MDCR_MDS1	0x0200	// MD1 pin
#define	MDCR_MDS2	0x0400	// MD2 pin
#define	MDCR_MDS3	0x0800	// MCU mode

// SYStem Control Register
#define	SYSCR		((volatile uint16_t *)0xfffdc2)
#define	SYSCR_DTCMD	0x0002
#define	SYSCR_RAME	0x0100
#define	SYSCR_EXPE	0x0200
#define	SYSCR_FETCHMD	0x0800
#define	SYSCR_MACS	0x2000

__BEGIN_DECLS
void mcu_init (void);
void mcu_info (void);
__END_DECLS


#endif
