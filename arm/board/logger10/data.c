
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
#include <sys/ringbuffer.h>
#include <sys/delay.h>

#include <string.h>

#include <reg.h>

#include "data.h"
#include "timecnt.h"
#include "lcd.h"
#include "local.h"
#include "storage.h"

#define	MAX_TRAILERS	4

STATIC int __data_current_speed;
STATIC int __data_current_lap;

void postprocess_lap (uint32_t);
void postprocess_speed (uint32_t);
void postprocess_accelerometer (uint32_t, uint32_t *);
void postprocess_distancemeter (uint32_t, uint32_t *);

struct interrupt_event
{
  // interrupt port is fixed to Port0
  const char *name;
  uint32_t event;
  uint32_t pin;
  uint32_t bit;
  uint32_t edge;
  void (*postprocess) (uint32_t);
} interrupt_event[] =
  {
    { "SPEED"	, DATA_TYPE_INTR_SPEED, 8, 1 << 8, GPIO_EDGE_F,
      postprocess_speed },
    { "LAP"	, DATA_TYPE_INTR_LAP,  10, 1 << 10,  GPIO_EDGE_F,
      postprocess_lap },
#if 0
    { "RPM"	, DATA_TYPE_INTR_RPM,  0, 1 << 0,  GPIO_EDGE_R, NULL },
    { "Misc"	, DATA_TYPE_INTR_MISC,  1, 1 << 1,  GPIO_EDGE_F, NULL },
#endif
  };

struct trailer_event
{
  uint32_t event;
  void (*postprocess) (uint32_t, uint32_t *);
  int n;	// # of trailers.
  uint32_t buf[MAX_TRAILERS];
} trailer_event[] =
  {
#ifdef ENABLE_ACCELEROMETER
    { DATA_TYPE_ACCELEROMETER, postprocess_accelerometer, 1, { 0 } },
#endif
#ifdef ENABLE_DISTANCEMETER
    { DATA_TYPE_STROKE_F, postprocess_distancemeter, 1, { 0 } },
#endif
  };

#define	INTR_RBUF_SIZE	64

STATIC struct thread_control data_tc __attribute__ ((aligned (4)));
STATIC void data_thread (uint32_t);
STATIC void data_idle (void) __attribute__((noreturn));
STATIC void data_sampling (void) __attribute__((noreturn));
STATIC void data_stop (void) __attribute__((noreturn));
STATIC int data_thread_ready;

STATIC uint8_t __buf_intr[RBUF_NOLOCK_SIZE (INTR_RBUF_SIZE)];
STATIC rbuf_nolock_t buf_intr;

STATIC struct
{
  uint32_t t_speed;	// previous speed sensor interrupt time.
  uint32_t t_lap;	// previous lap sensor interrupt time.
  int total_lap;
  int magnet_count;
  int cnt;
  uint32_t *p;	// DMA buffer.
} __data;

 void data_process (uint32_t);
STATIC void data_dma_buffer_setup (void);
STATIC void data_interrupt_event_init (void);

thread_t
data_init ()
{
  thread_t th;

  th = thread_create_no_stack (&data_tc, "data", data_thread, 0);
  thread_start (th);

  while (!data_thread_ready)
    thread_block (NULL);

  return th;
}

void
data_thread (uint32_t arg __attribute__((unused)))
{
  // storage:PRI_HIGH, data:PRI_APPHI, controller: PRI_LOW
  thread_priority (current_thread, PRI_APPHI);
  data_interrupt_event_init ();

  vic_interrupt_enable (VIC_EINT3, VIC_IRQ);

  // Logger timer. Free count. 1msec.
  __timer_config (TIMER3, TIMER_COUNTER_RESET, TIMER_MSEC);
  timer_start_nointr (TIMER3);
  buf_intr = rbuf_nolock_init (__buf_intr, INTR_RBUF_SIZE);

  // Logger setting.
  iprintf ("display: %s, magnet=%d\n", dipsw_laptime ? "lap time" : "elapsed",
	   dipsw_nmagnet);
  memset (&__data, 0, sizeof __data);
  // Data buffer. directly write to DMA buffer.
  data_dma_buffer_setup ();

      // Now OK to log.
  data_thread_ready = TRUE;
  // Setuped. wakeup master.
  thread_wakeup (shell_th);
  // This thread resumed from data_main.
  thread_block (data_idle);
  // NOTREACHED
}

void
data_dma_buffer_setup ()
{

  storage_buffer_flip ();
  __data.p = storage_current_buffer ();
  // Fill end marker.
  memset (__data.p, 0xff, 512);
  __data.cnt = 0;
}

void
data_idle ()
{
  intr_enable ();

  thread_block (log_status_flag == LOG_STATUS_IDLE ? data_idle : data_sampling);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
data_stop ()
{
  intr_enable ();

  // Flush
  printf ("flushing... %x\n", __data.p);
  *__data.p = 0xffffffff;	// End marker.

  // For next logging.
  memset (&__data, 0, sizeof __data);
  // Change current buffer to write buffer.
  data_dma_buffer_setup ();

  __data_current_speed = 0;
  assert (storage_th);
  // Write out remaining data.
 retry:
  thread_wakeup_once (storage_th);
  if (log_status_flag == LOG_STATUS_STOP)
    {
      thread_block (&&retry);
    }
  printf ("flush done.\n");
  thread_block (data_idle);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
data_sampling ()
{
  uint8_t buf[INTR_RBUF_SIZE];
  uint32_t *q;
  size_t sz, i;
  cpu_status_t s;

  // continuation is called with interrupt disable. enable again.
  intr_enable ();
  if (log_status_flag != LOG_STATUS_SAMPLING)
    data_stop ();

  // Read from interrupt context buffer.
  s = intr_suspend ();
  sz = rbuf_nolock_read (buf_intr, buf, INTR_RBUF_SIZE);
  intr_resume (s);

  // Move data to DMA buffer.
  for (i = 0, q = (uint32_t *)buf; i < sz; i += sizeof (uint32_t))
    {
      data_process (*q);

      // Copy to DMA buffer.
      *__data.p++ = *q++;
      __data.cnt += sizeof (uint32_t);

      // If buffer is full, write out to SD-card.
      if (__data.cnt == 512)
	{
	  // Flip data bank.
	  data_dma_buffer_setup ();
	  // Kick storage thread. storage thread priority is higher than myself.
	  thread_wakeup (storage_th);
	}
    }
  thread_block (data_sampling);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
data_interrupt_event_init ()
{
  struct interrupt_event *p = interrupt_event;
  int n = sizeof interrupt_event / sizeof (interrupt_event[0]);
  int i;

  for (i = 0; i < n; i++, p++)
    {
      int pin = p->pin;

      mcu_pin_select2 (0, pin, PIN_FUNC0);// GPIO
      mcu_pin_mode (0, pin, PIN_PULLUP);
      gpio_dir (0, pin, GPIO_INPUT);
      gpio_interrupt (0, pin, p->edge);
      printf ("%s Port0_%d %sing-edge\n", p->name, pin,
	      p->edge & GPIO_EDGE_F ?  "Fall" : "Ris");
    }
}

void
eint3_intr ()
{
  struct interrupt_event *p;
  uint32_t r, cause;
  int i, n = sizeof interrupt_event / sizeof (interrupt_event[0]);

  r = *T3TC;	// current time.
  cause = *IO0IntStatF;	// cause.

  // Process interrupt event. (no trailers)
  for (i = 0, p = interrupt_event; i < n; i++, p++)
    {
      if (cause & p->bit)
	{
	  r |= p->event;
	  //	  DPRINTF ("%s %x\n", p->name, *FIO0PIN);
	}
    }

  if (log_status_flag == LOG_STATUS_SAMPLING)
    {
      // Push to interrupt ringbuffer.
      rbuf_nolock_write (buf_intr, (uint8_t *)&r, sizeof (uint32_t));
      thread_wakeup_once (data_th);
    }
  else
    {
      // For debug. process data here.
      for (i = 0, p = interrupt_event; i < n; i++, p++)
	{
	  if ((r & p->event) && p->postprocess)
	    p->postprocess (r);
	}
    }

  // Clear interrupt.
  *IO0IntClr = *IO0IntStatF;
}

void
postprocess_speed (uint32_t t)
{
  uint32_t diff = t - __data.t_speed;

  if (diff)
    {
      __data_current_speed = (CIRCUMFERENCE * 36) / (t - __data.t_speed);
      __data.t_speed = t;
      //      DPRINTF ("%d km/h\n", __data_current_speed);
      // Display this
      //      lcd_data_misc ();
    }
}

void
postprocess_lap (uint32_t t)
{
  uint32_t lap = t - __data.t_lap;

  __data.t_lap = t;
  __data.total_lap += lap;

  // Start RTC if required.
  if (timecnt_start ())
    __data_current_lap = 0;

  if (++__data.magnet_count == dipsw_nmagnet)
    {
#ifdef DEBUG
      uint32_t msec, sec, min;
      uint32_t total_lap = __data.total_lap;
      min = total_lap / (1000 * 60);
      total_lap -= min * 1000 * 60;
      sec = total_lap / 1000;
      total_lap -= sec * 1000;
      msec = total_lap;
      iprintf ("lap %d'%d.%d (%d)\n", min, sec, msec, dipsw_nmagnet);
#endif
      lcd_data_laptime (__data.total_lap);
      lcd_data_misc (0, __data_current_lap);
      __data.total_lap = 0;
      __data.magnet_count = 0;
      __data_current_lap++;
    }

}

void
data_process (uint32_t r)
{
  struct interrupt_event *ip;
  struct trailer_event *tp;
  int i, n;
  uint32_t t = r & ~(DATA_TYPE_MASK << DATA_TYPE_SHIFT);
  uint32_t type = r & (DATA_TYPE_MASK << DATA_TYPE_SHIFT);
  static int buf_cnt;
  static struct trailer_event *tev;

  // Trailer events.
  if (buf_cnt > 0)
    {
      tev->buf[tev->n - buf_cnt] = r;
      if (--buf_cnt == 0)
	{
	  tev->postprocess (tev->event, tev->buf);
	}
      return;
    }

  if (type & (DATA_TYPE_TRAILERS << DATA_TYPE_SHIFT))
    {
      n = sizeof trailer_event / sizeof (trailer_event[0]);
      for (i = 0, tp = trailer_event; i < n; i++, tp++)
	{
	  if (type == tp->event)
	    {
	      tev = tp;
	      buf_cnt = tp->n;
	      break;
	    }
	}
    }
  else
    {
      // Interrupt events.
      n = sizeof interrupt_event / sizeof (interrupt_event[0]);
      for (i = 0, ip = interrupt_event; i < n; i++, ip++)
	{
	  // interrupt event can multiplex.
	  if ((type & ip->event) && ip->postprocess)
	    ip->postprocess (t);
	}
    }
}

void
data_event_trailers (uint32_t event, uint32_t *buf, int n)
{
  if (log_status_flag != LOG_STATUS_SAMPLING)
    return;

  int i;
  uint32_t r = *T3TC | event;	// current time.
  cpu_status_t s = intr_suspend ();

  rbuf_nolock_write (buf_intr, (uint8_t *)&r, sizeof (uint32_t));
  for (i = 0; i < n; i++, buf++)
    rbuf_nolock_write (buf_intr, (uint8_t *)buf, sizeof (uint32_t));

  thread_wakeup_once (data_th);
  intr_resume (s);
}

int
data_current_lap ()
{

  return __data_current_lap;
}

int
data_current_speed ()
{

  return __data_current_speed;
}
