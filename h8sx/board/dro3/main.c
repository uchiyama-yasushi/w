
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
#include <sys/thread.h>

#include <sys/shell.h>
#include <sys/board.h>
#include <sys/delay.h>

#include <1655/timer.h>

#include <sys/debug.h>	// bitdisp

SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (zero);

// On-board LED
STATIC void led_init (void);
STATIC void led (bool);

STATIC void ioport_init (void);

// Strawberry Linux 7segment-LED unit is connected to SCI6
STATIC void sci6_init (void);
STATIC void sci6_putc (int8_t);
STATIC int led_format (uint8_t *, int32_t);
STATIC void led_format_diff (uint8_t *, int32_t, int32_t);

// Caliper signal is compatible with SHAN
STATIC int32_t shan_data_format (int32_t);
void caliper_zero (void);	// Called from ext_irq4@caliper_reset_switch.S

// Interrupt thread.
STATIC struct thread_control caliper_tc __attribute__ ((aligned (4)));
STATIC void caliper_thread (uint32_t);
STATIC void caliper_postprocess (void) __attribute__((noreturn));
STATIC bool caliper_thread_ready;

// Data shared with interrupt handler.
thread_t caliper_manager_thread;
// interrupt handler common data.
#define	SHAN_AXIS_MAX	2
int32_t shan_data[SHAN_AXIS_MAX];
uint8_t shan_data_updated;
uint8_t shan_error;
uint8_t shan_error_cause;
/*
  3     Axis1 sampling error
  2	Axis1 frame error
  1     Axis0 sampling error
  0	Axis0 frame error
 */

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | DELAY_CALIBRATE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  led_init ();
  timer_init ();
}

void
board_main (uint32_t arg __attribute__((unused)))
{

  SHELL_COMMAND_REGISTER (test);
  SHELL_COMMAND_REGISTER (zero);

  printf ("DRO3 Build %s %s\n", __DATE__, __TIME__);

  //  shell_set_device (SERIAL, SERIAL, TRUE);

  caliper_manager_thread = thread_create_no_stack
    (&caliper_tc, "caliper manager", caliper_thread, (uint32_t)current_thread);
  thread_start (caliper_manager_thread);
  while (!caliper_thread_ready)
    thread_block (NULL);
  printf ("OK!\n");
  intr_enable ();
  led (TRUE);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
led_init ()
{

  BSET (3, PM_DDR);
}

void
led (bool on)
{
  if (on)
    BCLR (3, PM_DR);
  else
    BSET (3, PM_DR);
}

void
caliper_thread (uint32_t argc)
{

  thread_priority (current_thread, PRI_APPHI);
  ioport_init ();
  sci6_init ();
  caliper_thread_ready = TRUE;
  thread_wakeup ((thread_t)argc);
  intr_enable ();
  thread_block (caliper_postprocess);
  // NOTREACHED
}

void
caliper_postprocess ()
{
  uint8_t led_buf[16];
  int i, j;

  uint8_t updated = shan_data_updated;
  int32_t data[SHAN_AXIS_MAX];
  static int32_t val[SHAN_AXIS_MAX];
  data[0] = shan_data[0];
  data[1] = shan_data[1];
  shan_data_updated = 0;
  intr_enable ();

  for (j = 0; j < SHAN_AXIS_MAX; j++)
    {
      uint8_t bit = 1 << j;

      if (updated & bit)
	{
	  val[j] = shan_data_format (data[j]);
	}
      int k = led_format (led_buf, val[0]);

      led_format_diff (led_buf, val[0], val[1]);

      for (i = 0; i < k; i++)
	sci6_putc (led_buf[i]);
    }

  thread_block (caliper_postprocess);
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

uint32_t
test (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{

  while (/*CONSTCOND*/1)
    {
      ibitdisp8 (*P5_PORT);
    }

  return 0;
}

uint32_t
zero (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{

  // Zero reset caliper.
  cpu_status_t s = intr_suspend ();
  caliper_zero ();
  intr_resume (s);

  return 0;
}

void
ioport_init ()
{
  cpu_status_t s = intr_suspend ();

  // Configure port and pin.
  // P50(IRQ0) - CLK0
  // P51(IRQ1) - DAT0
  // P52(IRQ2) - CLK1
  // P53(IRQ3) - DAT1
  //
  // P54(IRQ4) - Caliper reset switch (default pull-down)
  //
  // PA0       - CLK0 pullup
  // PA1       - CLK1 pullup

  *PA_DDR = 0;		// Input port.

  // Caliper 0
  BSET (0, PFCRC);	// P50 as #IRQ0-B pin
  BSET (0, P5_ICR);	// Enable intput buffer. (clock)
  BSET (1, P5_ICR);	// Enable intput buffer. (data)

  // Caliper 1
  BSET (2, PFCRC);	// P52 as #IRQ2-B pin
  BSET (2, P5_ICR);	// Enable intput buffer. (clock)
  BSET (3, P5_ICR);	// Enable intput buffer. (data)

  // Caliper reset switch
  BSET (4, PFCRC);	// P52 as #IRQ2-B pin
  BSET (4, P5_ICR);	// Enable intput buffer. (clock)

  // Configure interrupt controller
  // Caliper 0
  intc_priority (INTPRI_IRQ0, INTPRI_7);
  intc_irq_sense (0, ISCR_FALLING_EDGE);
  *INTC_ISR &= ~ISR_IRQ0F; // Clear pending interrupt.
  *INTC_IER |= IER_IRQ0E; // Enable.

  // Caliper 1
  intc_priority (INTPRI_IRQ2, INTPRI_7);
  intc_irq_sense (2, ISCR_FALLING_EDGE);
  *INTC_ISR &= ~ISR_IRQ2F; // Clear pending interrupt.
  *INTC_IER |= IER_IRQ2E; // Enable.

  // Caliper reset switch
  intc_priority (INTPRI_IRQ4, INTPRI_7);
  intc_irq_sense (4, ISCR_RISING_EDGE);	// Active HI
  *INTC_ISR &= ~ISR_IRQ4F; // Clear pending interrupt.
  *INTC_IER |= IER_IRQ4E; // Enable.

  intr_resume (s);
}

void
sci6_init ()
{
  // Start SCI6 module
  peripheral_power (PWR_SCI6, TRUE);

  // Disable transmit/receive.
  *SCI (6, SCR) &= ~(SCR_TE | SCR_RE);

  // Setup Input buffer.
  // SCR_TE changes PM0 to SCI6 TxD
  BCLR (1, PM_ICR);	//PM0: SCI6 TxD
  BSET (1, PM_ICR);	//PM1: SCI6 RxD

  // Clock select
  *SCI (6, SCR) &= ~(SCR_CKE0 | SCR_CKE1); // Peripheral clock / 1

  // Tx/Rx Format.
  *SCI (6, SCMR) = 0;	// Clocked synchronous mode.
  *SCI (6, SMR)	= 0;	// 8N1

  // Bit rate. CKS=0, CKS=1, ABCS=0 (ABCS bit:SCI2,5,6 only)
  // Peripheral clock 24MHz 9600bps
  *SCI (6, BRR) = 77;

  // Wait at least 1bit interval.
  udelay (105);	//1000000/9600 = 104.166

  // Enable transmit/receive.
  *SCI (6, SCR) |= (SCR_TE | SCR_RE);
}

void
sci6_putc (int8_t c)
{

  while ((*SCI (6, SSR) & SSR_TDRE) == 0)
    ;
  *SCI (6, TDR) = c;
  *SCI (6, SSR) &= ~SSR_TDRE;
}

int
led_format (uint8_t *buf, int32_t bin)
{
  int i, k, s;
  int32_t j;
  bool start = FALSE;
  int sign_point = 0;

  if ((s = sign (bin)) < 0)
    bin *= -1;

  /* Skip 3 digit See led_format_diff () */
  for (i = 10000, k = 3; i > 0; i /= 10, k++)
    {
      j = bin / i;
      bin -= j * i;
      if (!start)
	{
	  buf[k] = ' ';
	  if (j || k > 4 /*0.xx*/)
	    {
	      start = TRUE;
	      sign_point = k - 1;
	    }
	}

      if (start)
	{
	  buf[k] = '0' + j;
	  if (i == 100)
	    buf[++k] = '.';
	}
    }
  if (s < 0)
    buf[sign_point] = '-';

  buf[k++] = '\r';

  return k;
}

void
led_format_diff (uint8_t *buf, int32_t val0, int32_t val1)
{
  int diff = val0 - val1;
  int s;
  int val;

  // Set the left 3 digit.
  if ((s = sign (diff)) < 0)
    {
      diff *= -1;
      buf[0] = '-';
    }
  else
    {
      buf[0] = ' ';
    }
  // Too large.
  if (diff > 99)
    buf[0] = 'E';

  diff %= 100;
  if ((val = diff / 10))
    buf[1] = '0' + val;
  else
    buf[1] = ' ';
  diff -= val * 10;
  buf[2] = '0' + diff;
}

// caliper specific preformat.
int32_t
shan_data_format (int32_t a)
{

  if (a & 0x800000)
    a |= 0xff000000;
  a = (a * 63) / 508;

  return a;
}

void
caliper_zero ()
{
  iprintf ("zero!\n");

  *PA_DDR = 0x3;// Change to Output port.
  *PA_DR = 0x3;
  mdelay (400);
  *PA_DR = 0;
  *PA_DDR = 0;	// Return to Input port. (Hi-Z pin. To stabilize clock signal)
}
