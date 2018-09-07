
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

#ifndef _LPC23XX_RTC_H_
#define	_LPC23XX_RTC_H_
// Interrupt Location Register
#define	RTC_ILR		((volatile uint32_t *)0xe0024000)	// 3
#define	 ILR_RTCCIF	0x1	// Counter Increment
#define	 ILR_RTCALF	0x2	// ALarm
#define	 ILR_RTSSF	0x4	// Counter Increment Sub-Seconds.
#define	 ILR_MASK	0x7

// Clock Tick Counter
#define	RTC_CTC		((volatile uint32_t *)0xe0024004)	//15 RO

// Clock Control Register
#define	RTC_CCR		((volatile uint32_t *)0xe0024008)	// 4
#define	 CCR_CLKEN	0x01	// Clock Enable
#define	 CCR_CTCRST	0x02	// CTC Reset
#define	 CCR_CLKSRC	0x10	// Clock source. Prescaler/ RTCX1, RTCX2 pin

// Counter Increment Interrupt Register
#define	RTC_CIIR	((volatile uint32_t *)0xe002400c)	// 8
#define	 CIIR_IMSEC	0x01
#define	 CIIR_IMMIN	0x02
#define	 CIIR_IMHOUR	0x04
#define	 CIIR_IMDOM	0x08
#define	 CIIR_IMDOW	0x10
#define	 CIIR_IMDOY	0x20
#define	 CIIR_IMMON	0x40
#define	 CIIR_IMYEAR	0x80

// Alarm Mask Register
#define	RTC_AMR		((volatile uint32_t *)0xe0024010)	// 8
#define	 AMR_SEC	0x01
#define	 AMR_MIN	0x02
#define	 AMR_HOUR	0x04
#define	 AMR_DOM	0x08
#define	 AMR_DOW	0x10
#define	 AMR_DOY	0x20
#define	 AMR_MON	0x40
#define	 AMR_YEAR	0x80
#define	 AMR_MASK	0xff

// Consolidated Time Register 0
#define	RTC_CTIME0	((volatile uint32_t *)0xe0024014)	//32 RO
#define	 CTIME0_SEC_MASK	0x3f	// 0-59
#define	 CTIME0_SEC_SHIFT	0
#define	 CTIME0_MIN_MASK	0x3f	// 0-59
#define	 CTIME0_MIN_SHIFT	8
#define	 CTIME0_HOUR_MASK	0x1f	// 0-23
#define	 CTIME0_HOUR_SHIFT	16
#define	 CTIME0_DOW_MASK	0x7	// 0-6
#define	 CTIME0_DOW_SHIFT	24

// Consolidated Time Register 1
#define	RTC_CTIME1	((volatile uint32_t *)0xe0024018)	//32 RO
#define	 CTIME1_DOM_MASK	0x1f	// 1-(28,29,30,31)
#define	 CTIME1_DOM_SHIFT	0
#define	 CTIME1_MONTH_MASK	0xf	// 1-12
#define	 CTIME1_MONTH_SHIFT	8
#define	 CTIME1_YEAR_MASK	0xfff	// 0-4095
#define	 CTIME1_YEAR_SHIFT	16

// Consolidated Time Register 2
#define	RTC_CTIME2	((volatile uint32_t *)0xe002401c)	//32 RO
#define	 CTIME2_DOY_MASK	0xfff	// 1-(365,366)
#define	 CTIME2_DOY_SHIFT	0

// Seconds Counter
#define	RTC_SEC		((volatile uint32_t *)0xe0024020)	// 6
// Minutes Register
#define	RTC_MIN		((volatile uint32_t *)0xe0024024)	// 6
// Hours Register
#define	RTC_HOUR	((volatile uint32_t *)0xe0024028)	// 5
// Day of Month Register
#define	RTC_DOM		((volatile uint32_t *)0xe002402c)	// 5
// Day of Week Register
#define	RTC_DOW		((volatile uint32_t *)0xe0024030)	// 3
// Day of Year Register
#define	RTC_DOY		((volatile uint32_t *)0xe0024034)	// 9
// Mounth Register
#define	RTC_MONTH	((volatile uint32_t *)0xe0024038)	// 4
// Years Register
#define	RTC_YEAR	((volatile uint32_t *)0xe002403c)	//12

// Counter Increment select mask for Sub-Second interrupt
#define	RTC_CISS	((volatile uint32_t *)0xe0024040)	// 8
#define	 CISS_CNT16	0x0	// 488usec @32.768kHz
#define	 CISS_CNT32	0x1	// 977usec @32.768kHz
#define	 CISS_CNT64	0x2	// 1.95msec @32.768kHz
#define	 CISS_CNT128	0x3	// 3.9msec @32.768kHz
#define	 CISS_CNT256	0x4	// 7.8msec @32.768kHz
#define	 CISS_CNT512	0x5	// 15.6msec @32.768kHz
#define	 CISS_CNT1024	0x6	// 31.25msec @32.768kHz
#define	 CISS_CNT2048	0x7	// 62.5msec @32.768kHz
#define	 CISS_SUBSECENA	0x80	// sub-second interrupt enable.

// Alarm value for Seconds
#define	RTC_ALSEC	((volatile uint32_t *)0xe0024060)	// 6
// Alarm value for Minutes
#define	RTC_ALMIN	((volatile uint32_t *)0xe0024064)	// 6
// Alarm value for Hour
#define	RTC_ALHOUR	((volatile uint32_t *)0xe0024068)	// 5
// Alarm value for Day of Month
#define	RTC_ALDOM	((volatile uint32_t *)0xe002406c)	// 5
// Alarm value for Day of Week
#define	RTC_ALDOW	((volatile uint32_t *)0xe0024070)	// 3
// Alarm value for Day of Year
#define	RTC_ALDOY	((volatile uint32_t *)0xe0024074)	// 9
// Alarm value for Months
#define	RTC_ALMON	((volatile uint32_t *)0xe0024078)	// 4
// Alarm value for Year
#define	RTC_ALYEAR	((volatile uint32_t *)0xe002407c)	//12

// Prescaler value,integer portion (for PCLK source)
#define	RTC_PREINT	((volatile uint32_t *)0xe0024080)	//13
#define	 PREINT_MASK	0xfff

// Prescaler value,fractional portion (for PCLK source)
#define	RTC_PREFRAC	((volatile uint32_t *)0xe0024084)	//15
#define	 PREFRAC_MASK	0x7fff

__BEGIN_DECLS
#include <sys/rtc_ymdhms.h>
// Power on/off
void rtc_init (void);
void rtc_fini (void);
// Counter start
void rtc_start (enum vic_irq_type);
void rtc_stop (void);
// Coutner set
bool rtc_time_set (struct rtc_ymdhms *);
bool rtc_time_get (struct rtc_ymdhms *);
// Interrupt.
bool rtc_alarm (struct rtc_ymdhms *, bool);
bool rtc_counter_incr (enum ymdhms, bool);
bool rtc_subsecond (uint32_t, bool);
// Util.
void rtc_print (void);
__END_DECLS
#endif
