
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
#include <sys/thread.h>
#include <sys/timer.h>

#include <string.h>
#include <reg.h>

#include "local.h"
#include "data.h"
#include "accelerometer.h"
#include "distancemeter.h"
#include "lcd.h"
#include "storage.h"

#define	LED7_BLANK	0x00

#define	LED7_0		0x3f
#define	LED7_1		0x06
#define	LED7_2		0x5b
#define	LED7_3		0x4f
#define	LED7_4		0x66
#define	LED7_5		0x6d
#define	LED7_6		0x7d
#define	LED7_7		0x07
#define	LED7_8		0x7f
#define	LED7_9		0x67

#define	LED7_DP		0x80
#define	LED7_MINUS	0x40

#define	LED7_A		0x77
#define	LED7_b		0x7c
#define	LED7_c		0x58
#define	LED7_d		0x5e
#define	LED7_E		0x79
#define	LED7_F		0x71
#define	LED7_L		0x38
#define	LED7_r		0x50
#define	LED7_o		0x5c

uint8_t digit_pattern[] =
  {
    LED7_0,
    LED7_1,
    LED7_2,
    LED7_3,
    LED7_4,
    LED7_5,
    LED7_6,
    LED7_7,
    LED7_8,
    LED7_9,
    LED7_A,
    LED7_b,
    LED7_c,
    LED7_d,
    LED7_E,
    LED7_F,
  };

uint8_t display_buffer[3] =
  {
    LED7_o, LED7_o, LED7_o
  };
int dip_switch;

thread_t controller_th;

STATIC struct thread_control controller_tc __attribute__((aligned (4)));
STATIC thread_func controller_thread;
STATIC continuation_func controller_main __attribute__((noreturn));

STATIC void controller_button (void);
STATIC void controller_led (void);
STATIC void controller_led_set_status (void);

STATIC void controller_device_init (void);
#if 0	// currently unused
STATIC void display_digit (int, int);
#endif
STATIC int controller_thread_ready;

bool dipsw_test0;
bool dipsw_test1;
int dipsw_nmagnet;
bool dipsw_laptime;

void
controller_init ()
{

  controller_th = thread_create_no_stack (&controller_tc, "controller",
					  controller_thread, 0);
  thread_start (controller_th);

  while (!controller_thread_ready)
    thread_block (NULL);
}

void
controller_thread (uint32_t arg __attribute__((unused)))
{
  intr_enable ();
#ifdef ENABLE_ACCELEROMETER
  accelerometer_init ();
#endif
#ifdef ENABLE_DISTANCEMETER
  distancemeter_init ();
#endif
  // Initialize GPIO
  controller_device_init ();
  // Update dip-switch setting.
  controller_dipswitch_update ();
  // Lazy polling thread.
  thread_priority (current_thread, PRI_LOW);
  controller_thread_ready = TRUE;
  thread_wakeup (shell_th);

  controller_main ();
  // NOTREACHED
}

void
controller_main ()
{
  intr_enable ();

  controller_button ();
  controller_led ();
#ifdef ENABLE_ACCELEROMETER
  uint32_t a;
  if (accelerometer_read (&a))
    {
      data_event_trailers (DATA_TYPE_ACCELEROMETER, &a, 1);
    }
#endif
#ifdef ENABLE_DISTANCEMETER
  uint32_t d;
  if (distancemeter_read (&d))
    {
      data_event_trailers (DATA_TYPE_STROKE_F, &d, 1);
    }
#endif

  timer_block (100000, controller_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
controller_device_init ()
{
  int i, j;

  *FIO3MASK = 0;
  *FIO4MASK = 0;

  // 4bit Dip switch(P3_0...P3_3)  + 1 Toggle switch (P3_4)
  for (i = 0; i < 5; i++)
    {
      mcu_pin_select2 (3, i, PIN_FUNC0);	// GPIO
      mcu_pin_mode (3, i, PIN_PULLUP);
      *FIO3DIR &= ~(1 << i);	// Input
    }
  // 3digit 7-segment LED sink.
  for (i = 5; i < 8; i++)
    {
      j = 1 << i;
      mcu_pin_select2 (3, i, PIN_FUNC0);	// GPIO
      mcu_pin_mode (3, i, PIN_PULLDOWN);
      *FIO3DIR |= j;	// Output
      *FIO3PIN |= j;	// High
    }
  // 7-segment LED soruce.
  for (i = 0; i < 8; i++)
    {
      j = 1 << i;
      mcu_pin_select2 (4, i, PIN_FUNC0);	// GPIO
      mcu_pin_mode (4, i, PIN_PULLUP);
      *FIO4DIR |= j;	// Output
      *FIO4PIN &= ~j;	// Low
    }
}

void
controller_dipswitch_update ()
{
  uint32_t r;

  r = *FIO3PIN & 0xf;	// dip-switch;
  dip_switch = ~r & 0xf;
  dipsw_test0 = dip_switch & 0x1;
  dipsw_test1 = dip_switch & 0x2;
  dipsw_laptime = dip_switch & 0x4;
  dipsw_nmagnet = (dip_switch & 0x8) ? 3 : 1;

}

void
controller_button ()
{
  static uint32_t old_r;
  uint32_t r, edge_f, edge_r;

  r = *FIO3PIN & 0x1f;	// dip-switch + toggle-switch
  edge_f = (r ^ old_r) & ~r;
  edge_r = (r ^ old_r) & r;
  old_r = r;
  dip_switch = ~r & 0xf;

  // Toggle switch's Falling edge starts logger # with specified by dip-switch.
  if (edge_f & 0x10)
    {
      logger_start ();
    }
  // Toggle switch's rising edge stop logger.
  else if (edge_r & 0x10)
    {
      logger_stop ();
    }
}

void
controller_led_set_status ()
{

  switch (log_status_flag)
    {
    case LOG_STATUS_IDLE:
      display_buffer[0] = LED7_1;
      display_buffer[1] = LED7_d;
      display_buffer[2] = LED7_L;
      break;
    case LOG_STATUS_STOP:
      display_buffer[0] = LED7_DP;
      display_buffer[1] = LED7_DP;
      display_buffer[2] = LED7_DP;
      break;
    case LOG_STATUS_SAMPLING:
      display_buffer[0] = digit_pattern[dipsw_nmagnet];
      display_buffer[1] = LED7_MINUS;
      display_buffer[2] = digit_pattern[current_log_number];
      break;
    }
}

#if 0	// currently unused.
void
display_digit (int d, int dot)
{
  int d100, d10;

  d100 = d / 100;
  d -= d100 * 100;
  d10 = d / 10;
  d -= d10 * 10;
  if (d100 > 9)
    d100 = 11;	//Error
  display_buffer[0] = digit_pattern[d100];
  display_buffer[1] = digit_pattern[d10];
  display_buffer[2] = digit_pattern[d];
  if (dot > 0)
    display_buffer[dot] |= LED7_DP;
}
#endif

void
controller_led ()
{
  static int digit;
  int d = digit % 3;

  // Setup display buffer.
  controller_led_set_status ();

  // Show LED.
  *FIO3PIN |= (0x7 << 5);	// Off 3-digit all.
  *FIO4PIN = display_buffer[d];
  *FIO3PIN &= ~(1 << (d + 5));
  digit++;
}
