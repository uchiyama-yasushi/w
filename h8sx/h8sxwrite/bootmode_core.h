
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

#ifndef _BOOTMODE_CORE_H_
#define	_BOOTMODE_CORE_H_

enum boot_program_state
  {
    DEVICE_SELECT = 0x11,
    CLOCK_MODE_SELECT = 0x12,
    BITRATE_SELECT = 0x13,
    STATE_TRANSITION = 0x1f,
    ERASE_STATE = 0x31,
    WRITE_ERASE_SELECT_WAIT = 0x3f,
    DATA_WAIT = 0x4f,
    ERASE_WAIT = 0x5f,
  };

#define	SUPPORT_DEVICE_INQUIRE		0x20
struct support_device_response
{
  uint8_t response;	//0x30
  uint8_t size;
  uint8_t n_device;
  uint8_t n_string;
  char device_code[4];
  char boot_prog_name[0];
} __attribute__((packed));

#define	DEVICE_SELECT_COMMAND		0x10
struct device_select_command
{
  uint8_t command;	//0x10
  uint8_t size;		//4
  uint8_t device_code[4];
  uint8_t checksum;
} __attribute__((packed));

#define	CLOCK_MODE_INQUIRE		0x21
struct clock_mode_response
{
  uint8_t response;	//0x31
  uint8_t size;
  uint8_t mode[0];
} __attribute__((packed));

#define	CLOCK_MODE_SELECT_COMMAND	0x11
struct clock_mode_select_command
{
  uint8_t command;	//0x11
  uint8_t size;		//1
  uint8_t mode;
  uint8_t checksum;
} __attribute__((packed));

#define	MULTIPLICATION_RATIO_INQUIRE	0x22
struct multiplication_ratio_response
{
  uint8_t response;	//0x32
  uint8_t size;
  uint8_t n_mult_type;
  uint8_t n_ratio;
  uint8_t ratio[0/*n_ratio + n_mult_type*/];
} __attribute__((packed));

#define	CLOCK_INQUIRE			0x23
struct range
{
  uint16_t min;
  uint16_t max;
} __attribute__((packed));

struct clock_response
{
  uint8_t response;	//0x33
  uint8_t size;
  uint8_t n_src_clock;
  struct range clock[0/*n_src_clock*/];
} __attribute__((packed));

#define	USERBOOT_MAT_INQUIRE		0x24
#define	USER_MAT_INQUIRE		0x25
struct area
{
  uint32_t start;
  uint32_t end;
} __attribute__((packed));

struct mat_response	// user or user_boot
{
  uint8_t response;	//0x34(user_boot)/0x35(user)
  uint8_t size;
  uint8_t n_area;
  struct area address[0/*n_area*/];
} __attribute__((packed));

#define	ERASED_BLOCK_INQUIRE		0x26
struct erased_block_response
{
  uint8_t response;	//0x36
  uint16_t size;
  uint8_t n_block;
  struct area block[0/*n_block*/];
} __attribute__((packed));

#define	WRITE_SIZE_INQUIRE		0x27
struct write_size_response
{
  uint8_t response;	//0x37
  uint8_t size;		//2
  uint16_t write_size;
} __attribute__((packed));

#define	BITRATE_SELECT_COMMAND		0x3f
struct bitrate_select_command
{
  uint8_t command;	//0x3f
  uint8_t size;
  uint16_t bitrate;
  uint16_t clock;
  uint8_t n_mult_type;
  uint8_t main_clock_ratio;
  uint8_t peripheral_clock_ratio;
  uint8_t checksum;
} __attribute__((packed));

#define	TRANSITION_WRITE_ERASE_STATE_COMMAND	0x40

#define	USER_BOOT_MAT_SELEC_COMMAND	0x42
#define	USER_MAT_SELECT_COMMAND		0x43

#define	WRITE_128BYTE_COMMAND		0x50
struct write_128byte_command
{
  uint8_t command;	//0x50
  uint32_t addr;
  uint8_t data[1024];	//XXX
  uint8_t checksum;
} __attribute__((packed));

#define	WRITE_END_COMMAND		0x50
struct write_end_command
{
  uint8_t command;	//0x50
  uint32_t addr;	//0xffffffff
  uint8_t checksum;
} __attribute__((packed));

#define	READ_COMMAND			0x52
struct read_command
{
  uint8_t command;	//0x52
  uint8_t size;		//9
  uint8_t area;		//0: user-boot MAT, 1: user MAT
  uint32_t addr;
  uint32_t read_size;
  uint8_t checksum;
} __attribute__((packed));

struct read_command_response
{
  uint8_t response;	//0x52
  uint32_t addr;
  uint8_t data[0];
};

#define	ERASE_SELECT_COMMAND		0xd0
struct block_erase_command
{
  uint8_t command;	//0x58
  uint8_t size;
  uint8_t block;
  uint8_t checksum;
} __attribute__((packed));

#define	BOOT_PROGRAM_STATUS_INQUIRE	0x4f
struct boot_program_status_response
{
  uint8_t response;	//0x5f
  uint8_t size;
  uint8_t status;
  uint8_t error;
  uint8_t checksum;
} __attribute__((packed));

bool check_status (enum boot_program_state);
bool initialize (int, int);
void *inquire (uint8_t);
bool command (uint8_t *, ssize_t, enum boot_program_state);

#endif	// !_BOOTMODE_CORE_H_
