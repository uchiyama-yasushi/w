
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

#ifndef _BOARD_H_
#define	_BOARD_H_
#include <sys/types.h>

// Board specific initialize, entry routine.
// Program must provide theses routines.

enum boot_config
  {
    CONSOLE_ENABLE		= 0x01,	// Polling Serial I/O
    BUFFERED_CONSOLE_ENABLE	= 0x02,	// Interrupt/Threading Serial I/O
    RAM_CHECK			= 0x04,	// BSS check.
    DELAY_CALIBRATE		= 0x08,	// calibrate delay parameter.(paranoia)
  };

__BEGIN_DECLS
/* arguments is setted by kernel. */
void board_device_init (uint32_t/*boot_config which returns board_boot_config*/);
void board_main (uint32_t/*user defined context*/);
/* returns boot_config. user can add local definition. */
uint32_t board_boot_config (void);
void board_reset (uint32_t) __attribute__((noreturn));
__END_DECLS

#endif //_BOARD_H_
