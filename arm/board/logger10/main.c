
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
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <reg.h>

#include <string.h>
#include <stdlib.h>

#include "local.h"
#include "data.h"
#include "timecnt.h"
#include "lcd.h"
#include "storage.h"

SHELL_COMMAND_DECL (logger);
SHELL_COMMAND_DECL (ls);
SHELL_COMMAND_DECL (rm);
SHELL_COMMAND_DECL (debug);

thread_t storage_th;
thread_t data_th;
thread_t shell_th;
#ifdef ENABLE_GPS
thread_t gps_th;
#endif

int current_log_number;
enum log_status log_status_flag;

STATIC void onboard_led (int);

void
board_main (uint32_t arg __attribute__((unused)))
{
  shell_th = current_thread;

  shell_command_register (&logger_cmd);
  shell_command_register (&ls_cmd);
  shell_command_register (&rm_cmd);
  shell_command_register (&debug_cmd);

  // Contoller thread
  controller_init ();
  // Storage thread.
  storage_th = storage_init ();
  // Data thread needs DMA buffer information of storage thread.
  data_th = data_init ();
#ifdef ENABLE_GPS
  // GPS thread.
  gps_th = gps_init ();
#endif

  // D/A converter. P-LAP signal threshold voltage.
  dac_init ();
  dac_set (2000);

  onboard_led (TRUE);
  lcd_data_misc (dipsw_nmagnet, storage_nfile ());

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
board_reset (uint32_t cause __attribute__((unused)))
{

  // Stop timer.
  timer_fini ();
  // Stop DMA
  dma_fini ();
  // Stop RTC
  rtc_fini ();

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
onboard_led (int on)
{

  if (on)
    *FIO1PIN &= ~(1 << 18);	// P1.18 low.
  else
    *FIO1PIN |= (1 << 18);
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  // on-board LED
  // GPIO power: always enabled.
  *SCS |= SCS_GPIOM;	// Fast GPIO mode.
  *FIO1DIR = 1 << 18;	// P1.18 connected to on-board LED.
  *FIO1MASK = 0;
  //LED on
  *FIO1PIN &= ~(1 << 18);	// P1.18 low.

  vic_init ();
  timer_init ();
  dma_init ();
  rtc_init ();
  rtc_start (VIC_IRQ);

  lcd_app_init ();
}

uint32_t
logger (int32_t argc, const char *argv[])
{

  if (argc < 2)
    {
      switch (log_status_flag)
	{
	default:
	  printf ("bogus state.\n");
	  assert (0);
	  break;
	case LOG_STATUS_IDLE:
	  printf ("idle.\n");
	  break;
	case LOG_STATUS_SAMPLING:
	  printf ("sampling.\n");
	  break;
	case LOG_STATUS_STOP:
	  printf ("stop process.\n");
	  break;
	}
      return 0;
    }

  if (strcmp (argv[1], "start") == 0)
    {
      logger_start ();
      return 0;
    }

  if (strcmp (argv[1], "stop") == 0)
    {
      logger_stop ();
      return 0;
    }

  return 0;
}

bool
logger_start ()
{
  char logfile_name[] = "logx.dat";
  int i;

  for (i = 0; i < 8; i++)
    {
      logfile_name[3] = '0' + i;
      if (storage_file_open (logfile_name))
	break;
    }
  if (i == 8)
    {
      printf ("can't open log file.\n");
      return FALSE;
    }
  current_log_number = i;
  printf ("LOG start. open %s\n", logfile_name);

  log_status_flag = LOG_STATUS_SAMPLING;

  return TRUE;
}

void
logger_stop ()
{

  if (log_status_flag != LOG_STATUS_SAMPLING)
    return;

  // Stop RTC
  timecnt_stop ();

  // Stop data, storage thread.
  log_status_flag = LOG_STATUS_STOP;

  printf ("LOG stopping.\n");
  while (log_status_flag != LOG_STATUS_IDLE)
    {
      thread_wakeup_once (data_th);
      thread_rotate_ready_queue ();
    }
  printf ("LOG stopped.\n");
}

uint32_t
ls (int32_t argc __attribute__((unused)),
    const char *argv[] __attribute__((unused)))
{

  storage_ls ();
  return 0;
}

uint32_t
rm (int32_t argc __attribute__((unused)),
    const char *argv[] __attribute__((unused)))
{

  if (argc < 1)
    return 0;

  storage_rm (argv[1]);
  return 0;
}

uint32_t
debug (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
#ifdef ENABLE_DISTANCEMETER
  extern int i2c_debug;

  i2c_debug ^= 1;
#endif
  printf ("state=%d\n", log_status_flag);

  return 0;
}

