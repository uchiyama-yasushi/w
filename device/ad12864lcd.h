
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

// aitendo AD-12864-SPI LCD Module.
#ifndef _DEVICE_AD12864LCD_H_
#define	_DEVICE_AD12864LCD_H_
// Display ON/OFF
#define	DISPLAY_ON	0xaf
#define	DISPLAY_OFF	0xae
// Display Start Line Set
#define	START_LINE	0x40
#define	 START_LINE_MASK	0x3f
// Page Address Set
#define	PAGE_ADDR	0xb0
#define	 PAGE_ADDR_MASK		0x7
// Column Address Set		[DOUBLE BYTE COMMAND]
#define	COLUMN_ADDR_HI	0x10	// upper 4bit.
#define	COLUMN_ADDR_LO	0x00	// lower 4bit
#define	 COLUMN_ADDR_MASK	0x7f
// Read Status
#define	STATUS_BUSY	0x80
#define	STATUS_ADC	0x40
#define	STATUS_ON	0x20
#define	STATUS_RESET	0x10

// ADC Select
#define	ADC_NORMAL	0xa0
#define	ADC_REVERSE	0xa1
// Display Normal/Reverse
#define	DISPLAY_REVERSE	0xa6
#define	DISPLAY_NORMAL	0xa7
// Display All Points ON/OFF
#define	ALL_POINT_ON	0xa5
#define	ALL_POINT_OFF	0xa4
// LCD Bias Set
#define	LCD_BIAS_1_9	0xa2
#define	LCD_BIAS_1_8	0xa3
// Read Modify Write Set
#define	READMODIFYWRITE_SET	0xe0
#define	READMODIFYWRITE_END	0xee
// Reset
#define	RESET		0xe2
// Common Outpu Mode Select
#define	SCAN_DIR_NORMAL		0xc0	// COM0->COM63
#define	SCAN_DIR_REVERSE	0xc8	// COM63->COM0
// Power Controller Set
#define	POWER		0x28
#define	 POWER_BOOSTER		0x04
#define	 POWER_REGULATOR	0x02
#define	 POWER_FOLLOWER		0x01
// V5 Voltage Regulator Internal Registor Ratio Set
#define	REGULATOR	0x20
#define	 REGULATOR_MASK	0x07
// Electronic Volume Set	[DOUBLE BYTE COMMAND]
#define	BRIGHTNESS	0x81
#define	 BRIGHTNESS_MASK	0x3f
// Static Indicator		[DOUBLE BYTE COMMAND]
#define	STATIC_INDICATOR_ON	0xac
#define	STATIC_INDICATOR_OFF	0xad
#define	 INDICATOR_OFF		0x00
#define	 INDICATOR_BLINK_FAST	0x00	// 0.5sec
#define	 INDICATOR_BLINK_SLOW	0x00	// 1sec.
#define	 INDICATOR_ON		0x00	// constantly on

// 64*128 point total 1KB
#define	VRAM_PAGE_SIZE		128
#define	VRAM_NPAGE		8

__BEGIN_DECLS
void lcd_display_init (void);
void lcd_command_putc (int8_t);
void lcd_data_putc (int8_t);
void lcd_set_page (int);
void lcd_set_column (int);
void lcd_contrast (int);
void lcd_display_font (int, int, int);// direct draw

// V-RAM ops.
void lcd_vram_clear_page (int);
void lcd_vram_transfer (void);
void lcd_vram_font (int, int, int, bool);
uint8_t *lcd_vram_get_page (int);
__END_DECLS

#endif
