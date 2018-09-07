
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

#ifndef _BOOTMODE_INTERFACE_H_
#define	_BOOTMODE_INTERFACE_H_

bool support_device_inquire (struct flash_config *);
bool clock_inquire (struct flash_config *);
bool clock_mode_inquire (struct flash_config *);
bool multiplication_ratio_inquire (struct flash_config *);
bool userboot_mat_inquire (struct flash_config *);
bool user_mat_inquire (struct flash_config *);
bool erased_block_inquire (struct flash_config *);
bool write_size_inquire (struct flash_config *);

bool device_select (const char *);
bool clock_mode_select (uint8_t);
bool new_bitrate_select (struct flash_config *, uint8_t);
bool transition_write_erase_state (void);
bool user_mat_select (void);
bool write_data (addr_t, uint8_t *);
bool write_end (void);
bool read_data (uint32_t, uint32_t);

#endif	//!_BOOTMODE_INTERFACE_H_
