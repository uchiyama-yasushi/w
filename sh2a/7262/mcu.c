
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
  MD_BOOT1 MD_BOOT0	Boot mode
     0        0		CS0 memory
     0        1		SPI ch0 fast speed
     1        0		NAND flash memory
     1        1		SPI ch0 slow speed
 */
STATIC int __mcu_boot_mode;

void
mcu_init (enum boot_mode boot_mode)
{
  __mcu_boot_mode = boot_mode;

  pwr_init ();
  intc_init ();
  // Set Clock.
  // Interface 2010/6 appendix board is clock mode 1
  clk_init (CLK_MODE_1, 48, FRQCR_IFC_x1, FRQCR_PFC_x1_6);
}

void
mcu_info ()
{

  clk_info ();
  pwr_info ();
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
