
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

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

/*
  MDS3 MDS2 MDS1 MDS0 MCU operating mode
   1    1    0    1          1		13	user-boot mode
   1    1    0    0          2		12	boot mode
   0    1    0    0          3		4	boundary scan single chip mode
   0    0    1    0          4		2	ROM disabled extended mode 16bit
   0    0    0    1          5		1	ROM disabled extended mode 8bit
   0    1    0    1          6		5	ROM enabled extended mode
   0    1    0    0          7		4	single chip mode
 */

void
mcu_init ()
{

  intc_init ();
}

void
mcu_info ()
{
  int8_t MDpins[7] = { 13, 12, 4, 2, 1, 5, 4 };
  uint16_t mdcr = *MDCR >> 8 & 0xf;
  uint16_t syscr = *SYSCR;
  uint8_t intcr= *INTCR;
  size_t i;

  for (i = 0; i < sizeof MDpins; i++)
    if (mdcr == MDpins[i])
      break;
  if (i == sizeof MDpins)
    {
      printf ("Invalid MDCR %x\n", *MDCR);
      return;
    }
  printf ("MCU mode: ");
  if (i == 2)
    printf ("3 or 7\n");
  else
    printf ("%d\n", i + 1);

  printf ("DTC: %s-address mode\n",syscr & SYSCR_DTCMD ? "short" : "full");
  printf ("Internal RAM: %sabled\n", syscr & SYSCR_RAME ? "en" : "dis");
  printf ("External bus: %sabled\n",syscr & SYSCR_EXPE ? "en" : "dis");
  printf ("Instruction fetch: %dbit\n",syscr & SYSCR_FETCHMD ? 16 : 32);
  printf ("MAC instruction: %ssaturation mode",syscr & SYSCR_MACS ? "" : "non-");
  printf ("\n");
  printf ("Interrput Control mode: %d\n",
	  (intcr >> INTCR_MODE_SHIFT) & INTCR_MODE_MASK);
}

void
board_reset (uint32_t cause)
{

  switch (cause)
    {
    case 0:
      __asm volatile ("trapa #0");
      break;
    case 1:
      __asm volatile ("trapa #1");
      break;
    case 2:
      __asm volatile ("trapa #2");
      break;
    case 3:
      __asm volatile ("trapa #3");
      break;
    }
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}
