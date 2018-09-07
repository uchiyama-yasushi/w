
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

#ifndef _PCAT_BIOS_H_
#define	_PCAT_BIOS_H_
#include <sys/console.h>

#define	PCAT_BIOS_LOAD_ADDR	0x7c00
#define	PCAT_MBR_ADDR		0x7dbe

//----------------------------------------------------------------------
//	0x13
//----------------------------------------------------------------------
#define	BIOS_DISKIO_RETRY_MAX	3

#define	INT13_RESET		0x00
#define	INT13_READ		0x02
#define	INT13_WRITE		0x03

// Ext.INT13h.
#define	INT13_EXT_PRESENT	0x41
#define	INT13_EXT_READ		0x42
#define	INT13_EXT_WRITE		0x43
#define	INT13_EXT_VERIFY	0x44
#define	INT13_EXT_LOCK		0x45
#define	INT13_EXT_EJECT		0x46
#define	INT13_EXT_SEEK		0x47
#define	INT13_EXT_GET_PARAM	0x48
#define	INT13_EXT_DISKCHANGE	0x49
#define	INT13_EXT_HW_CONFIG	0x4e

// AH read:0x42 write:0x43
// AL 0,1: Disk verify off, 2: Disk verify on
// DL drive #.
// DS:SI address of disk_address_packet.
//
// Return value: AH contains error code when CF is set.
struct disk_address_packet
{
  uint8_t size_of_packet;	//0x10(buffer32) or 0x18(buffer64)
  uint8_t reserved0;
  uint8_t nblocks;
  uint8_t reserved1;

  uint32_t buffer32;
  uint32_t lba[2];
  uint32_t buffer64[2];	// when buffer32 is 0xffffffff, use this.
} __attribute__ ((packed));

// 0x48: Extended Read Drive Parameters
struct disk_parameters
{
  uint16_t size;
  uint16_t flags;
  uint32_t n_cylinders;
  uint32_t n_heads;
  uint32_t n_sectors_per_track;
  uint32_t n_sector[2];
  uint16_t n_bytes_per_sector;
  uint32_t edd_config_ptr;
} __attribute__ ((packed));

struct bios_hdd
{
  struct disk_address_packet disk_address_packet;
  uint8_t drive;
  uint8_t command;
  uint16_t command_addr;
  uint16_t packet_addr;
  // BIOS buffer. must be able to access by ES:BX.
  uint8_t buffer[0];
} __attribute__((packed));

// INT13h
// Application defined.
struct bios_int13_packet
{
  uint8_t drive;	//0 %dl
  uint8_t head;		//1 %dh
  uint8_t track;	//2 %ch
  uint8_t sector;	//3 %cl
  uint8_t n_sector;	//4 %al
  uint8_t command;	//5 %ah
  uint16_t segment;	//6 %es
  uint16_t offset;	//8 %bx
} __attribute__((packed));

struct bios_fdd
{
  struct bios_int13_packet packet;
  // BIOS access buffer. must be able to access by ES:BX.
  uint16_t packet_addr;
  int32_t drive;
  uint8_t buffer[0];
} __attribute__((packed));

//----------------------------------------------------------------------
//	0x1a
//----------------------------------------------------------------------
// Clock : 18.2Hz
#define	INT1A_CLOCK_GET		0x00
#define	INT1A_CLOCK_SET		0x01
#define	INT1A_RTCTIME_GET	0x02
#define	INT1A_RTCTIME_SET	0x03
#define	INT1A_RTCDATE_GET	0x04
#define	INT1A_RTCDATE_SET	0x05

struct bios_int1a_rtctime_packet
{
  uint8_t summer_time;	// %dl (BCD)
  uint8_t sec;		// %dh (BCD)
  uint8_t min;		// %cl (BCD)
  uint8_t hour;		// %ch (BCD)
  uint8_t pad;		// %al
  uint8_t command;	// %ah
  uint16_t ret;		// %ax
} __attribute__((packed));

struct bios_int1a_rtcdate_packet
{
  uint8_t day;		// %dl (BCD)
  uint8_t month;	// %dh (BCD)
  uint8_t year;		// %cl (BCD)
  uint8_t century;	// %ch (BCD)
  uint8_t pad;
  uint8_t command;
  uint16_t ret;
} __attribute__((packed));

struct bios_int1a_clock_packet
{
  uint16_t cnt_low;
  uint16_t cnt_high;
  uint8_t pad;
  uint8_t command;
  uint16_t ret;
} __attribute__((packed));

union bios_int1a_packet
{
  struct bios_int1a_rtctime_packet rtc_time;
  struct bios_int1a_rtcdate_packet rtc_date;
  struct bios_int1a_clock_packet clock;
  uint32_t u32[2];
};

__BEGIN_DECLS
typedef int (bios_func_t)(int, int) __attribute__((regparm (2)));

int __call16 (bios_func_t *, int, int) __attribute__((regparm (3)));
int __lcall16 (bios_func_t *, int, int) __attribute__((regparm (3)));
#ifdef BOOTSTRAP_GDT
#define	call16(a, b, c)	__call16(a, b, c)
#else
#define	call16(a, b, c) __lcall16(a, b, c)
#endif

bios_func_t bios_int15;
bios_func_t bios_int1a;
bios_func_t bios_delay_calibration;
bios_func_t bios_delay;
void bios_delay32 (int) __attribute__((regparm (1)));

bios_func_t bios_com_init;
bios_func_t bios_com_putc;
bios_func_t bios_com_getc;
bios_func_t bios_com_poll;

bios_func_t bios_vga_mode;
bios_func_t bios_vga_putc;
bios_func_t bios_kbd_getc;
bios_func_t bios_kbd_poll;

bios_func_t bios_int13_ext;
bios_func_t bios_int13_ext_present;
bios_func_t bios_int13_rw;
bios_func_t bios_int13;

// Protect-mode interface.
enum console_device
  {
    SERIAL,
    NORMAL,
  };
void bios_console_init (enum console_device);

void *bios_disk_init (void *, uint8_t);
bool bios_disk_drive (void *, uint8_t);
bool bios_disk_read (void *, uint8_t *, daddr_t);
bool bios_disk_read_n (void *, uint8_t *, daddr_t, int);
bool bios_disk_write (void *, uint8_t *, daddr_t);
bool bios_disk_write_n (void *, uint8_t *, daddr_t, int);
void bios_disk_reset (uint8_t);
void bios_disk_info (void *, uint8_t);

void *bios_fdd_init (void *, uint8_t);
bool bios_fdd_drive (void *, uint8_t);
bool bios_fdd_read (void *, uint8_t *, daddr_t);
bool bios_fdd_read_n (void *, uint8_t *, daddr_t, int);
bool bios_fdd_write (void *, uint8_t *, daddr_t);
bool bios_fdd_write_n (void *, uint8_t *, daddr_t, int);

extern inline void
bios_disk_reset (uint8_t drive)
{

  call16 (bios_int13, INT13_RESET, drive);
}

int32_t bios_rtc_get (void);
uint32_t bios_timer_get (void);
bool bios_timer_set (uint32_t);

extern uint8_t boot_drive;
__END_DECLS
#endif
