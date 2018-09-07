
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

#include <sys/system.h>
#include <sys/types.h>
#include <sys/console.h>

struct
{
  uint32_t bit;
  const char *name;
} eflags_bits[] = {
  // (S) Status (C) Control
  { 1 << 0,   "CF" },	// Carry(S)
  { 1 << 2,   "PF" },	// Parity(S)
  { 1 << 4,   "AF" },	// Auciliary Carry(S)
  { 1 << 6,   "ZF" },	// Zero(S)
  { 1 << 7,   "SF" },	// Sign(S)
  { 1 << 8,   "TF" },	// Trap
  { 1 << 9,   "IF" },	// Interrupt
  { 1 << 10,  "DF" },	// Direction(C)
  { 1 << 11,  "OF" },	// Overflow(S)
  { 1 << 14,  "NT" },	// Nested Task
  { 1 << 16,  "RF" },	// Resume Flag
  { 1 << 17,  "VM" },	// Virtual 8086 Mode
  { 1 << 18,  "AC" },	// Alignment Check
  { 1 << 19,  "VIF" },	// Virtual Interrupt Flag
  { 1 << 20,  "VIP" },	// Virtual Interrupt Pending
  { 1 << 21,  "ID" },	// ID
};

void
eflags_dump (uint32_t eflags)
{
  int i;

  for (i = 0; i < sizeof eflags_bits / sizeof eflags_bits[0]; i++)
    {
      if (eflags & eflags_bits[i].bit)
	printf ("%s ", eflags_bits[i].name);
    }
  printf ("\n");
}

