
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

/*

Lshift 12
Rshift 59
control 14
tab d
escape 76
L-alt 11
R-alt e0 11
L-super 67 fb
R-super 64 f9
backspace 66
delete 71
ins 70
return 5a
z1a
x22
c21
v2a
b32
n31
m3a
,41
.49
/4a
a1c
s1b
d23
f2b
g34
h33
j3b
k42
l4b
;4c
'52
q15
w1d
e24
r2d
t2c
y35
u3c
i43
o44
p4d
[54
]5b
116
21e
326
425
52e
636
73d
83e
946
045
-4e
=55
\5d
`e

overrun:0xff

*/
#include <sys/system.h>
#include <sys/types.h>
#include <sys/console.h>
#include <x86/io.h>
#include <dev/pckbc.h>
#include <pcat/interrupt.h>
#include <dev/i8259.h>

#include <sys/thread.h>///XXX

void
pckbc_intr_enable (bool enable)
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

  outb(0x64, 0x60); // keyboard command write.
  r0 = enable ? 0x03 : 0x00;
  outb(0x60, r0); // write to output port.
}

int
kbd_intr ()
{
  uint8_t r;
  inb (0x60, r);
  extern thread_t app_th;	//XXX

  switch (r)
    {
    case 0xf0:
      printf ("released\n");
      break;
    default:
      printf ("wakeup %s\n", app_th->name);
      thread_wakeup (app_th);	//XXX
      printf ("%x\n", r);
      break;
    }

  return 0;
}

void
keyboard_init ()
{
  uint16_t r;

  interrupt_handler_install (1, kbd_intr);
  pckbc_intr_enable (TRUE);
  r = i8259_get_mask ();
  r &= ~(1 << 1);
  i8259_set_mask (r);
}
