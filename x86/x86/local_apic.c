
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
#include <x86/local_apic.h>

void
l_apic_enable (int id)
{

  *L_APIC_ID = id << L_APIC_ID_SHIFT;
  *L_APIC_SVR &= ~SVR_VECTOR_MASK;	// use vector 0
  *L_APIC_SVR |= SVR_APIC_ENABLE;
  printf ("Local APIC ID%d enabled.\n", *L_APIC_ID >> L_APIC_ID_SHIFT);
}

void
l_apic_init ()
{
  uint32_t r;

  r = *L_APIC_VER;
  printf ("Local APIC version %d, LVT=%d\n",
	  r & L_APIC_VER_MASK,
	  (r & L_APIC_VER_MAXLVT) >> L_APIC_VER_MAXLVT_SHIFT);
  l_apic_enable (0);

  *L_APIC_LVT_LINT0 &= 0xfffe00ff;// not masked, edge, active high
  *L_APIC_LVT_LINT0 |= 0x00005700;// ExtInt
  printf ("LVT1:%x\n", *L_APIC_LVT_LINT0);

  *L_APIC_LVT_LINT1 &= 0xfffe00ff;// not masked, edge, active high
  *L_APIC_LVT_LINT1 |= 0x00005400;// NMI
  printf ("LVT2:%x\n", *L_APIC_LVT_LINT1);

  *L_APIC_LVT_ERROR = 210;

}
