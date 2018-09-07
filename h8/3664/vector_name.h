
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

// H8/3664 exeception vector define.

static struct
{
  int addr;
  const char *name;
} const vector [] = {
  { 0x00, "RESET" },
  { 0x02, "reserved1" },
  { 0x04, "reserved2" },
  { 0x06, "reserved3" },
  { 0x08, "reserved4" },
  { 0x0a, "reserved5" },
  { 0x0c, "reserved6" },
  { 0x0e, "NMI" },
  { 0x10, "TRAPA#0" },
  { 0x12, "TRAPA#1" },
  { 0x14, "TRAPA#2" },
  { 0x16, "TRAPA#3" },
  { 0x18, "Adress Break" },
  { 0x1a, "Sleep" },
  { 0x1c, "IRQ0" },
  { 0x1e, "IRQ1" },
  { 0x20, "IRQ2" },
  { 0x22, "IRQ3" },
  { 0x24, "Wakeup" },
  { 0x26, "Timer A" },
  { 0x28, "reserved20" },
  { 0x2a, "Timer W" },
  { 0x2c, "Timer V" },
  { 0x2e, "SCI3" },
  { 0x30, "I2C" },
  { 0x32, "A/D" },
};
