
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
#include <stdlib.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <cpu.h>
#include <reg.h>

#include <string.h>
#include <sys/thread.h>
#include <sys/delay.h>
#include <sys/timer.h>

#include <bfs_simple.h>
#include <pcat_mbr.h>
#include <sys/disklabel_netbsd.h>
#include <sys/mmc.h>

#include <mci_dma.h>

SHELL_COMMAND_DECL (ini);
SHELL_COMMAND_DECL (read);
SHELL_COMMAND_DECL (write);
SHELL_COMMAND_DECL (fs);
SHELL_COMMAND_DECL (ls);
SHELL_COMMAND_DECL (test);

STATIC struct mci_dma dma;
thread_func app0_thread;
thread_t app_th;
struct thread_control app0_tc;

//#define	UART3_TEST

void
board_main (uint32_t arg)
{
  printf ("%s: arg=%x\n", __FUNCTION__, arg);
  shell_command_register (&ini_cmd);
  shell_command_register (&read_cmd);
  shell_command_register (&write_cmd);
  shell_command_register (&fs_cmd);
  shell_command_register (&test_cmd);
  shell_command_register (&ls_cmd);

#ifdef UART3_TEST
  // PCLK 18.0000MHz 9615bps error 0.16%
  struct uart_clock_conf uart3 = { 0, 78, 1 | (2 << 4), CCLK4 };
  uart_init (UART3, &uart3, FALSE);
#endif

  app_th = thread_create_no_stack (&app0_tc, "app0", app0_thread, 0x11133aab);
  thread_start (app_th);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

static void
led_blink ()
{
  static int i;

  if (i ^= 1)
    GPIO_PIN_CLR (1, 18);
  else
    GPIO_PIN_SET (1, 18);
}

continuation_func app0_main __attribute__((noreturn));
void
app0_thread (uint32_t arg)
{

  rtc_start (VIC_IRQ);
  rtc_counter_incr (SEC, TRUE);

  iprintf ("%s: arg=%x\n", __FUNCTION__, arg);

  while (/*CONSTCOND*/1)
    {
      thread_block (NULL);
      iprintf ("wakeup!\n");
      led_blink ();
    }

  // NOTREACHED
}

void
app0_main ()
{
  led_blink ();
  thread_block (app0_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
rtc_intr_counter_incr ()
{

  thread_wakeup (app_th);
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  // Stop timer.
  timer_fini ();

  cpu_reset ();
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | DELAY_CALIBRATE | RAM_CHECK | BUFFERED_CONSOLE_ENABLE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // on-board LED
  // GPIO power: always enabled.
  mcu_fast_gpio (TRUE);
  gpio_dir (1, 18, GPIO_OUTPUT); // P1.18 connected to on-board LED.
  *FIO1MASK = 0;
  //LED on
  GPIO_PIN_CLR (1, 18);

  timer_init ();
  dma_init ();
  rtc_init ();
  vic_init ();

  dac_init ();
}

STATIC struct block_io_ops __sdcard;
STATIC struct mci_dma dma;

uint32_t
test (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
#ifdef UART3_TEST
  char s[] = "11 15 9.8\r";
  char *p;

  for (p = s; *p; p++)
    uart_putc (UART3, *p);
#else
  if (argc < 2)
    return 0;
  int mV = atoi (argv[1]);
  printf ("Set DAC %dmV\n", mV);
  dac_set (mV);

#endif
  return 0;
}

uint32_t
ini (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  // Initialize MCI module.
  if (mmc_init (&__sdcard))
    {
      printf ("SD card initialize failed.\n");
    }
  // Initialize SD-card.
  mci_card_init (__sdcard.cookie);

  mci_dma_alloc (&dma);

  return 0;
}

uint32_t
read (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  if (!__sdcard.cookie)
    ini (0, 0);

  if (!__sdcard.read (__sdcard.cookie, dma.read_buffer, atoi (argv[1])))
    return 0;

  int i, j, k;
  for (j = k = 0; j < 32; j++)
    {
      for (i = 0; i < 16; i++, k++)
	printf ("%x ", dma.read_buffer[k]);
      printf ("\n");
    }

  return 0;
}

uint32_t
write (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  int data, lba;

  if (!__sdcard.cookie)
    ini (0, 0);
  data = atoi (argv[2]);
  lba = atoi (argv[1]);

  memset (dma.read_buffer, data, dma.read_buffer_size);

  if (!__sdcard.write (__sdcard.cookie, dma.read_buffer, lba))
    printf ("failed.\n");

  printf ("write %d to lba %d\n", data, lba);

  return 0;
}

struct bfs *filesystem;

uint32_t
fs (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{
  int error;

  if (filesystem)
    {
      printf ("already mounted.\n");
      return 0;
    }
  if ((error = mmc_init (&__sdcard)))
    {
      printf ("SD card initialize failed.\n");
      return error;
    }


  mci_dma_alloc (&dma);
  filesystem = bfs_simple_init (&__sdcard, 0, dma.read_buffer,
				dma.fs_control_block);
  return 0;
}

uint32_t
ls (int32_t argc __attribute__((unused)),
    const char *argv[] __attribute__((unused)))
{
  if (!filesystem)
    {
      printf ("no filesystem\n");
      return 0;
    }

  bfs_dump (filesystem);
  return 0;
}
