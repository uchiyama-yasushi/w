
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

#ifndef _SG12232_H_
#define	_SG12232_H_

/* AKIZUKI SG12232 122x32 graphic LCD (SUNLIKE) */

// UD01 drives with H8 BSC(8bit 3state+ wait 3state). connected Area 6
#define	LCD_DATA	((volatile uint8_t *)0xc0005)
#define	LCD_DATA_0	((volatile uint8_t *)0xc0005)
#define	LCD_DATA_1	((volatile uint8_t *)0xc0006)
#define	LCD_DATA_BOTH	((volatile uint8_t *)0xc0004)
#define	LCD_CMD		((volatile uint8_t *)0xc0001)
#define	LCD_CMD_0	((volatile uint8_t *)0xc0001)
#define	LCD_CMD_1	((volatile uint8_t *)0xc0002)
#define	LCD_CMD_BOTH	((volatile uint8_t *)0xc0000)

// Command
#define	CMD_DISPLAY_ON		0xae
#define	 DISPLAY_ON	0x1
#define	 DISPLAY_OFF	0x0
#define	CMD_START_LINE		0xc0	// 0-31
#define	CMD_PAGE_ADDR		0xb8	// 0-3
#define	CMD_COLUMN_ADDR		0x00	// 0-79
#define	CMD_ADC_SELECT		0xa0
#define	 ADC_RIGHT	0x0	// default.
#define	 ADC_LEFT	0x1
#define	CMD_STATIC_DRIVE	0xa4
#define	CMD_DUTY_SELECT		0xa8
#define	DUTY_SELECT_16	0
#define	DUTY_SELECT_32	1	// fixed. don't change.

#define	CMD_READ_MODIFY_WRITE	0xe0
// when read modify write mode, can't set column.
#define	CMD_END			0xee
// end read modify write mode.
#define	CMD_RESET		0xe2

// Status
#define	STAT_BUSY		0x80
#define	STAT_ADC		0x40
#define	STAT_ONOFF		0x20
#define	STAT_RESET		0x10

#define	LCD_PAGE_MAX	4
#define	LCD_COLUMN_MAX	61

#include <stdio.h>
__BEGIN_DECLS
struct _file *lcd_init (void);
void lcd_clear (uint8_t);
// Graphic interface.
void lcd_dot (int, int, bool);
void lcd_line (int, int, int, int);
void lcd_circle (int, int, int);
// Character interface.
void lcd_putc (int8_t);
void lcd_home (void);
extern const uint8_t font_clR8x8_data[];
#define	EXTERN_INLINE static inline
EXTERN_INLINE void
lcd_write (volatile uint8_t *addr, uint8_t data)
{

  *addr = data;
}

EXTERN_INLINE uint8_t
lcd_read (volatile uint8_t *addr)
{

  return *addr;
}
__END_DECLS
#endif
