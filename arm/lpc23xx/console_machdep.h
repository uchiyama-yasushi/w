
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

#ifndef _CONSOLE_MACHDEP_H_
#define	_CONSOLE_MACHDEP_H_
#include <reg.h>
// Device independent Serial I/O
__BEGIN_DECLS

// console for early boot. polling routines.
void boot_console_init (struct uart_clock_conf *);
__END_DECLS

/*
  Sample configuration. (output of uart_clock_conf.c)

 { 0, 52, 1 | (2 << 4), 0 }; // PCLK 12.0000MHz 9615bps error 0.16%
 { 0, 26, 1 | (2 << 4), 0 }; // PCLK 12.0000MHz 19231bps error 0.16%
 { 0, 13, 1 | (2 << 4), 0 }; // PCLK 12.0000MHz 38462bps error 0.16%
 { 0, 8, 5 | (8 << 4), 0 }; // PCLK 12.0000MHz 57692bps error 0.16%
 { 0, 4, 5 | (8 << 4), 0 }; // PCLK 12.0000MHz 115385bps error 0.16%
 { 0, 2, 5 | (8 << 4), 0 }; // PCLK 12.0000MHz 230769bps error 0.16%

 { 0, 78, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 9615bps error 0.16%
 { 0, 39, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 19231bps error 0.16%
 { 0, 19, 7 | (13 << 4), 0 }; // PCLK 18.0000MHz 38487bps error 0.23%
 { 0, 13, 1 | (2 << 4), 0 }; // PCLK 18.0000MHz 57692bps error 0.16%
 { 0, 6, 5 | (8 << 4), 0 }; // PCLK 18.0000MHz 115385bps error 0.16%
 { 0, 3, 5 | (8 << 4), 0 }; // PCLK 18.0000MHz 230769bps error 0.16%

 { 0, 96, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 9600bps error 0.00%
 { 0, 48, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 19200bps error 0.00%
 { 0, 24, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 38400bps error 0.00%
 { 0, 16, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 57600bps error 0.00%
 { 0, 8, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 115200bps error 0.00%
 { 0, 4, 0 | (1 << 4), 0 }; // PCLK 14.7456MHz 230400bps error 0.00%
*/

#endif
