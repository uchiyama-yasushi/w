
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

__BEGIN_DECLS
void null_handler (void) __attribute ((interrupt_handler)); // place holder.

void nmi (void) __attribute ((weak, alias ("null_handler")));
void trapa0 (void) __attribute ((weak, alias ("null_handler")));
void trapa1 (void) __attribute ((weak, alias ("null_handler")));
void trapa2 (void) __attribute ((weak, alias ("null_handler")));
void trapa3 (void) __attribute ((weak, alias ("null_handler")));

void irq0 (void) __attribute ((weak, alias ("null_handler")));
void irq1 (void) __attribute ((weak, alias ("null_handler")));
void irq2 (void) __attribute ((weak, alias ("null_handler")));
void irq3 (void) __attribute ((weak, alias ("null_handler")));
void irq4 (void) __attribute ((weak, alias ("null_handler")));
void irq5 (void) __attribute ((weak, alias ("null_handler")));

void itu0_a (void) __attribute ((weak, alias ("null_handler")));
void itu0_b (void) __attribute ((weak, alias ("null_handler")));
void itu0_ovf (void) __attribute ((weak, alias ("null_handler")));
void itu1_a (void) __attribute ((weak, alias ("null_handler")));
void itu1_b (void) __attribute ((weak, alias ("null_handler")));
void itu1_ovf (void) __attribute ((weak, alias ("null_handler")));
void itu2_a (void) __attribute ((weak, alias ("null_handler")));
void itu2_b (void) __attribute ((weak, alias ("null_handler")));
void itu2_ovf (void) __attribute ((weak, alias ("null_handler")));
void itu3_a (void) __attribute ((weak, alias ("null_handler")));
void itu3_b (void) __attribute ((weak, alias ("null_handler")));
void itu3_ovf (void) __attribute ((weak, alias ("null_handler")));
void itu4_a (void) __attribute ((weak, alias ("null_handler")));
void itu4_b (void) __attribute ((weak, alias ("null_handler")));
void itu4_ovf (void) __attribute ((weak, alias ("null_handler")));

void sci0_eri (void) __attribute ((weak, alias ("null_handler")));
void sci0_rxi (void) __attribute ((weak, alias ("null_handler")));
void sci0_txi (void) __attribute ((weak, alias ("null_handler")));
void sci0_tei (void) __attribute ((weak, alias ("null_handler")));
void sci1_eri (void) __attribute ((weak, alias ("null_handler")));
void sci1_rxi (void) __attribute ((weak, alias ("null_handler")));
void sci1_txi (void) __attribute ((weak, alias ("null_handler")));
void sci1_tei (void) __attribute ((weak, alias ("null_handler")));

void rfshc_cmi (void) __attribute ((weak, alias ("null_handler")));

void wdt_ovf (void) __attribute ((weak, alias ("null_handler")));

void ad_end (void) __attribute ((weak, alias ("null_handler")));

void dmac0_a (void) __attribute ((weak, alias ("null_handler")));
void dmac0_b (void) __attribute ((weak, alias ("null_handler")));
void dmac1_a (void) __attribute ((weak, alias ("null_handler")));
void dmac1_b (void) __attribute ((weak, alias ("null_handler")));
void
null_handler ()
{
}
__END_DECLS
