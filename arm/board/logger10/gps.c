
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

// NMEA-0183 parser.

#include <sys/system.h>
#include <sys/console.h>
#include <sys/thread.h>
#include <sys/fiber.h>
#include <sys/delay.h>
#include <sys/timer.h>
#include <reg.h>
#include <string.h>
#include <stdlib.h>

#include "local.h"
#include "data.h"
#include "lcd.h"


STATIC struct thread_control gps_tc __attribute__((aligned (4)));
STATIC thread_func gps_thread;
STATIC continuation_func gps_main __attribute__((noreturn));
STATIC int gps_thread_ready;

int gps_debug;

#define	FIFO_SIZE	64
#define	GPS_FIBER_STACK_SIZE	256

rbuf_nolock_t __recv_buf;
STATIC uint8_t buf_intr[RBUF_NOLOCK_SIZE (FIFO_SIZE)];
STATIC uint8_t buf_thread[FIFO_SIZE];
volatile size_t buf_thread_read_size;
uint8_t *buf_thread_read_p = buf_thread;
thread_t gps_th;

// Parser fibers
fiber_t fiber[3];
uint8_t fls[2][FIBER_STACK_SIZE (GPS_FIBER_STACK_SIZE)];
fiber_func gps_parser;
fiber_func gps_parse_nmea;
struct fiber myself;

__BEGIN_DECLS
void uart3_intr (void);
uint8_t gps_getc (fiber_t);
__END_DECLS

thread_t
gps_init ()
{
  thread_t th;

  th = thread_create_no_stack (&gps_tc, "gps", gps_thread, 0);
  thread_start (th);

  while (!gps_thread_ready)
    thread_block (NULL);

  return th;
}

void
gps_thread (uint32_t arg __attribute__((unused)))
{
  gps_th = current_thread;

  // PCLK 18.0000MHz 9615bps error 0.16%
  struct uart_clock_conf uart3 = { 0, 78, 1 | (2 << 4), CCLK4 };
  uart_init (UART3, &uart3, TRUE);
  // Interrupt context ring-buffer.
  __recv_buf = rbuf_nolock_init (buf_intr, FIFO_SIZE);

  // Create parser fiber.
  fiber[0] = fiber_init (&myself);
  fiber[1] = fiber_create (&myself, fls[0], GPS_FIBER_STACK_SIZE, gps_parser);
  fiber[2] = fiber_create (&myself, fls[1], GPS_FIBER_STACK_SIZE, gps_parse_nmea);
  fiber_twist (&myself, 2, fiber[1], fiber[2]);
  fiber_start (&myself, fiber[1]);
  fiber_start (&myself, fiber[2]);

  // Now OK to log.
  gps_thread_ready = TRUE;
  thread_priority (current_thread, PRI_LOW);
  thread_wakeup (shell_th);
  gps_main ();
  // NOTREACHED
}

void
gps_main ()
{
  // on entry interrupt disabled.

  buf_thread_read_size = rbuf_nolock_read (__recv_buf, buf_thread, FIFO_SIZE);
  intr_enable ();

  buf_thread_read_p = buf_thread;

  while (buf_thread_read_size)
    {
      fiber_yield (&myself, 0);
    }

  thread_block (gps_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

void
uart3_intr ()
{
  uint8_t c = *URBR (UART3_BASE);

  if (rbuf_nolock_write (__recv_buf, &c, 1) != 1)
    {
      DPRINTF ("UART3 overflow\n");
    }
  timer_schedule_func ((void (*)(void *))thread_wakeup_once, gps_th, 100);
}

uint8_t
gps_getc (fiber_t self)
{

  if (buf_thread_read_size == 0)
    {
      fiber_return_parent (self, 0);
    }
  assert (buf_thread_read_size);
  --buf_thread_read_size;

  return *buf_thread_read_p++;
}

uint8_t gps_buf[64];

void FIBER_FUNC_ATTRIBUTE
gps_parser (fiber_t myself)
{
  fiber_return_parent (myself, 0);
  // Next yield start here.
  uint8_t c;
  uint8_t *p = gps_buf;
  while (/*CONSTCOND*/1)
    {
      c = gps_getc (myself);
      *p++ = c;
      if (gps_debug)
	putc (c, altout);
      if ((c == ',') || (c == '\r') || (c == '\n'))
	{
	  *--p = '\0';
	  p = gps_buf;
	  fiber_yield (myself, 0);
	}
    }
  // NOTREACHED
}

uint32_t gga_buf[4];
uint32_t convert_data (uint8_t *);

void FIBER_FUNC_ATTRIBUTE
gps_parse_nmea (fiber_t myself)
{
  bool target = FALSE;
  int gga_cnt = 0;
  int gga_buf_cnt = 0;
  fiber_return_parent (myself, 0);
  // Next yield start here.

  while (/*CONSTCOND*/1)
    {
      if (gps_buf[0] == '$')
	{
	  if ((target = strcmp ((const char *)gps_buf, "$GPGGA") == 0))
	    {
	      if (gga_buf[2])	/* valid */
		{
		  lcd_data_gps_status ();
		  // push to SD-card
		  gga_buf[2] = gga_buf[3];
		  data_event_trailers (DATA_TYPE_GPS, gga_buf, 3);
		}
	      memset (gga_buf, 0, sizeof gga_buf);
	      gga_cnt = 0;
	      gga_buf_cnt = 0;
	    }
	}
      if (target)
	{
	  if ((gga_cnt == 2)/*latitude*/ || (gga_cnt == 4) /*longitude*/||
	      (gga_cnt == 6)/*valid*/ || (gga_cnt == 9)/*altitude*/)
	    {
	      uint32_t i = convert_data (gps_buf);
	      //	      iprintf ("[%s=>%d]\n", gps_buf, i);
	      gga_buf[gga_buf_cnt++] = i;
	    }
	  gga_cnt++;
	}

      fiber_yield (myself, 0);
    }
  // NOTREACHED
}

uint32_t
convert_data (uint8_t *b)
{
  uint8_t *p = b;
  uint8_t c;

  do
    {
      c = *b++;
    }
  while (c && c != '.');

  if (!c)
    return atoi ((const char *)p);

  --b;
  do
    {
      *b = *(b + 1);
    }
  while (*b++);
  return atoi ((const char *)p);
}

/*
GPGSA
 GNSS DOP and Active Satellites

$GPGSA,A,3,23,20,11,13,,,,,,,,,07.3,04.2,05.9*0D

  Satellites in View
$GPGSV,3,1,09,17,83,311,00,20,42,046,42,04,39,294,26,23,38,099,41*76
$GPGSV,3,2,09,13,27,143,38,28,23,203,00,11,18,092,41,32,17,043,*76
$GPGSV,3,3,09,02,09,280,00*41

  Recommended Minimum Specific GNSS Data
$GPRMC,084249.695,A,3539.4058,N,13939.2779,E,0000.00,344.21,251009,,*3A

  Course Over Ground and Ground Speed
$GPVTG,344.21,T,,M,0000.00,N,0000.00,K*50

  Time & Date
$GPZDA,084250.695,25,10,2009,,*5A

  Global Positioning System Fix Data
$GPGGA,084250.694,3539.4058,N,13939.2779,E,1,04,04.2,00039.2,M,0039.4,M,000.0,0000*49

$GPGGA,
084250.694, UTC
3539.4058,N,
13939.2779,E,
1,		Quality (0: can't receive)
04,		# of Satellites
04.2,		HDOP
00039.2,M,	altitude
0039.4,M,	altitude WGS-84
000.0,		Data age
0000		DPGS ID
*49		checksum


*/
