
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

// Real Time Clock module

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

#include <string.h>

typedef void (irq_handler_t)(void);
irq_handler_t __rtc_intr;
// Application override interrupt handler.
irq_handler_t rtc_intr_counter_incr __attribute__ ((weak, alias ("__rtc_intr")));
irq_handler_t rtc_intr_alarm __attribute__ ((weak, alias ("__rtc_intr")));
irq_handler_t rtc_intr_subsecond __attribute__ ((weak, alias ("__rtc_intr")));

enum rtc_state
  {
    RTC_OFF,
    RTC_ON,
    RTC_START,
  };

enum rtc_state __rtc_state;

STATIC void rtc_reset ();
STATIC void rtc_alarm_clear (void);

void
rtc_init ()
{
  struct rtc_ymdhms date;
  // Power on (On reset, the RTC is enabled.)
  mcu_peripheral_power (PCONP_PCRTC, TRUE);

  // Reset all.
  rtc_reset ();
  __rtc_state = RTC_ON;

  // RTC source clock.
  *RTC_CCR |= CCR_CLKSRC;	// 32.768kHz external oscillator.
  // If use PCLK, set RTC_PREINT and RTC_PREFRAC.

  memset (&date, 0, sizeof date);
  rtc_time_set (&date);
}

void
rtc_fini ()
{
  rtc_stop ();
  rtc_reset ();
  // Power off.
  mcu_peripheral_power (PCONP_PCRTC, FALSE);
  __rtc_state = RTC_OFF;
}

void
rtc_reset ()
{

  *RTC_CCR = 0;		// Clock off.
  *RTC_AMR = AMR_MASK;	// Alarm Mask all
  *RTC_CIIR = 0;	// Counter Increment Interrupt.
  *RTC_CISS = 0;	// Subsecond interrupt.
  // Clear pending interrupt.
  *RTC_ILR = ILR_RTCCIF | ILR_RTCALF | ILR_RTSSF;

  // Clear alarm
  rtc_alarm_clear ();
}

void
rtc_start (enum vic_irq_type irq_type)
{
  cpu_status_t s = intr_suspend ();
  assert (__rtc_state == RTC_ON);
  // Start RTC clock
  *RTC_CCR |= CCR_CLKEN;
  __rtc_state = RTC_START;

  // Interrupt Enable.
  if (irq_type == VIC_FIQ)
    {
      *VICIntSelect |= VIC_RTC; // FIQ
    }
  else
    {
      *VICIntSelect &= ~VIC_RTC; // IRQ
    }
  *VICIntEnable |= VIC_RTC;

  intr_resume (s);
}

void
rtc_stop ()
{

  // Interrupt Disable
  *VICIntEnable &= ~VIC_RTC;
  // Stop RTC clock.
  *RTC_CCR &= ~CCR_CLKEN;
  // Clear pending interrupt.
  *RTC_ILR = ILR_RTCCIF | ILR_RTCALF | ILR_RTSSF;
  __rtc_state = RTC_ON;
}

bool
rtc_time_set (struct rtc_ymdhms *date)
{

  *RTC_SEC = date->sec;
  *RTC_MIN = date->min;
  *RTC_HOUR = date->hour;
  *RTC_DOM = date->day;
  *RTC_DOW = date->wday;
  *RTC_DOY = 0; //XXX
  *RTC_MONTH = date->mon;
  *RTC_YEAR = date->year;

  return TRUE;
}

bool
rtc_time_get (struct rtc_ymdhms *date)
{

  date->sec = *RTC_SEC;
  date->min = *RTC_MIN;
  date->hour = *RTC_HOUR;
  date->day = *RTC_DOM;
  date->wday = *RTC_DOW;
  date->mon = *RTC_MONTH;
  date->year = *RTC_YEAR;

  return TRUE;
}

bool
rtc_alarm (struct rtc_ymdhms *date, bool on)
{
  assert (__rtc_state == RTC_START);

  // Clear Pending interrupt.
  *RTC_ILR |= ILR_RTCALF;

  if (on)
    {
      *RTC_ALSEC = date->sec;
      *RTC_ALMIN = date->min;
      *RTC_ALHOUR = date->hour;
      *RTC_ALDOM = date->day;
      *RTC_ALDOW = date->wday;
      *RTC_ALDOY = 0;//XXX
      *RTC_ALMON = date->mon;
      *RTC_ALYEAR = date->year;
      // Alarm mask.
      *RTC_AMR = 0;	// compare all.
    }
  else
    {
      *RTC_AMR = AMR_MASK;	// mask all.
      rtc_alarm_clear ();
    }

  return TRUE;
}

void
rtc_alarm_clear ()
{
  // Clear alarm
  *RTC_ALSEC = 0;
  *RTC_ALMIN = 0;
  *RTC_ALHOUR = 0;
  *RTC_ALDOM = 0;
  *RTC_ALDOW = 0;
  *RTC_ALDOY = 0;
  *RTC_ALMON = 0;
  *RTC_ALYEAR = 0;
  *RTC_AMR = AMR_MASK;	// mask all;
}

bool
rtc_counter_incr (enum ymdhms counter, bool on)
{
  // XXX This subroutine interface is something bogus.
  // XXX Should allow combination.
  uint32_t bit[] = { CIIR_IMYEAR, CIIR_IMMON, CIIR_IMDOM, CIIR_IMDOW,
		     CIIR_IMHOUR, CIIR_IMMIN, CIIR_IMSEC };
  cpu_status_t s = intr_suspend ();
  assert (__rtc_state == RTC_START);
  // Clear Pending interrupt.
  *RTC_ILR |= ILR_RTCCIF;

  if (on)
    {
      *RTC_CIIR = bit[counter];
    }
  else
    {
      *RTC_CIIR = 0;
    }
  intr_resume (s);

  return TRUE;
}

bool
rtc_subsecond (uint32_t subsecond_select, bool on)
{
  assert (__rtc_state == RTC_START);

  // SubSelSub-Second Select.
  *RTC_CISS = subsecond_select;

   // Clear Pending interrupt.
  *RTC_ILR |= ILR_RTSSF;

 // Enable/Disable interrupt.
  if (on)
    *RTC_CISS |= CISS_SUBSECENA;
  else
    *RTC_CISS &= ~CISS_SUBSECENA;

  return TRUE;
}

void
rtc_print ()
{
  struct rtc_ymdhms date;

  rtc_time_get (&date);

  iprintf ("%d %d %d %d %d %d %d\n", date.year, date.mon, date.day,
	   date.wday, date.hour, date.min, date.sec);
}

void
rtc_intr ()
{
  uint32_t r = *RTC_ILR;	// interrupt cause.

  *RTC_ILR |= (r & ILR_MASK);	// Clear interrupt.

  // Dispatch application defined handler.
  if (r & ILR_RTCCIF)
    rtc_intr_counter_incr ();
  if (r & ILR_RTCALF)
    rtc_intr_alarm ();
  if (r & ILR_RTSSF)
    rtc_intr_subsecond ();
}

void
__rtc_intr ()	// Place holder.
{

  rtc_print ();
}
