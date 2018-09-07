
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
#include <console_machdep.h>
#include <sys/shell.h>
#include <sys/srec.h>

SHELL_COMMAND_DECL (l);
SHELL_COMMAND_DECL (g);
SHELL_COMMAND_DECL (v);

void led (void);
void led_init (void);

void
startup ()
{
  extern char bss_start[], bss_end[];
  char *p;

  led_init ();

  // Clear BSS
  for (p = bss_start; p < bss_end; p++)
    *p = 0;

  boot_console_init (TRUE, 19); //Pck 24MHz 38400bps

  SHELL_COMMAND_REGISTER (l);
  SHELL_COMMAND_REGISTER (g);
  SHELL_COMMAND_REGISTER (v);

  iprintf ("SH7262 Simple Monitor Build %s %s\n", __DATE__, __TIME__);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

uint32_t
l (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  uint8_t c;
  srec_return_t ret = SREC_MISSING_START;
  int32_t err = -1;
  size_t sz;
#define	CCR1	((volatile uint32_t *)0xfffc1000)
#define	CCR2	((volatile uint32_t *)0xfffc1004)
  int8_t (*_getc) (void *) = altin->char_ops->getc;
  void *_getc_arg = altin->char_ops->ctx;

  srec_init ();
  iprintf ("CCR1=%lx CCR2=%lx\n", *CCR1, *CCR2);

  //  *CCR1 |= (1 << 1);// Write-thru
  for (sz = 0; (c = _getc (_getc_arg)); sz++)
    {
      if ((ret = srec_parser (c)) != SREC_OK)
	break;
    }

  iprintf ("Read %ld byte. %s\n", sz, srec_error_message[ret]);
  if (ret != SREC_EOF)
    {
      iprintf ("Failed.\n");
      goto failed;
    }
  iprintf ("Start address: %#lx\n", srec_start_addr);
  //  *CCR1 &= ~(1 << 1);// Write-back
  err = 0;

 failed:

  return err;
}

uint32_t
g (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{

  iprintf ("Start address: %#lx\n", srec_start_addr);
  // jump to user program.
  __asm volatile ("jsr @%0;nop" :: "r" (srec_start_addr));
  iprintf ("done.\n");

  return 0;
}

uint32_t
v (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  int i;

  for (i = 0; i < 0xc0; i++)
    printf ("%x ", *(volatile uint8_t *)(0x3c010000 + i));
  printf("\n------------------------------\n");
  for (i = 0; i < 0xc0; i++)
    printf ("%x ", *(volatile uint8_t *)(0x1c010000 + i));
  printf ("\n");

  return 0;
}

void
board_reset ()
{
}

void
led_init ()
{
#define	PCIOR0	((volatile uint16_t *)0xfffe3852)
#define	PCCR2	((volatile uint16_t *)0xfffe384a)
#define	PCDR	((volatile uint16_t *)0xfffe3856)

  *PCCR2 &= ~0x3;	// PC8
  *PCIOR0 |= (1 << 8);	// PC8 output
  *PCDR &= ~(1 << 8);
}

void
led ()
{
  static int blink;
#define	LED_OFF()	(*PCDR &= ~(1 << 8))
#define	LED_ON()	(*PCDR |= (1 << 8))

  if (blink ^= 1)
    *PCDR &= ~(1 << 8);
  else
    *PCDR |= (1 << 8);
}
