
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
#include <x86/io.h>
#include <dev/pckbc.h>

void
pckbc_outport_write (uint8_t val)
{
  uint8_t r0;

  // make sure that i/o buffer is empty.
  do
    {
      inb (0x60, r0);	// drain buffer
      inb (0x64, r0);	// get status
    }
  while (r0 & 0x01);	// output(i8042) buffer full
  do
    {
      inb (0x64, r0);
    }
  while (r0 & 0x02);	// input(i8042) Buffer Full

  outb(0x64, 0xd1); // output port write command.
  outb(0x60, val); // write to output port.
}

uint8_t
pckbc_outport_read ()
{
  uint8_t r0;

  // make sure that i/o buffer is empty.
  do
    {
      inb (0x60, r0);	// drain buffer
      inb (0x64, r0);	// get status
    }
  while (r0 & 0x01);	// output(i8042) buffer full
  do
    {
      inb (0x64, r0);
    }
  while (r0 & 0x02);	// input(i8042) Buffer Full

  outb(0x64, 0xd0); // output port read command.
#if 0
  do
    {
      inb (0x64, r0);
    }
  while (!(r0 & 0x01));	// output(i8042) Buffer Empty
#endif
  inb(0x60, r0);

  return r0;
}

void
pcat_reset ()
{

  pckbc_outport_write (0x00);
}
