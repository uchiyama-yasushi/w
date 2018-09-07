
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

#ifndef _MMC_H_
#define	_MMC_H_
__BEGIN_DECLS
// External Interface.
struct block_io_ops;
int mmc_init (struct block_io_ops *);
bool mmc_drive (void *, uint8_t);
bool mmc_read (void *, uint8_t *, daddr_t);
bool mmc_read_n (void *, uint8_t *, daddr_t, int);
bool mmc_write (void *, uint8_t *, daddr_t);
bool mmc_write_n (void *, uint8_t *, daddr_t, int);

// Raw command interface.
bool mmc_cmd_reset (void);
bool mmc_cmd_init (void);
void mmc_cmd_cid (void);
void mmc_cmd_csd (void);
void mmc_cmd_ocr (void);

// Machine dependent implementation.
void md_mmc_init (void);
int8_t md_mmc_getc (void);
void md_mmc_putc (int8_t);
void md_mmc_activate (void);
void md_mmc_deactivate (void);

__END_DECLS
#endif
