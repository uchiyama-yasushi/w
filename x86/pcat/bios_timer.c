
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
#include <sys/types.h>
#include <assert.h>
#include <sys/console.h>
#include <pcat/bios.h>
#include <string.h>

#define	BCD_TO_BIN(x)	(((x >> 4) & 0xf) * 10 + (x & 0xf))

int32_t
bios_rtc_get ()	//notyet
{
  union bios_int1a_packet packet;
  struct bios_int1a_rtcdate_packet *date = &packet.rtc_date;
  struct bios_int1a_rtctime_packet *time = &packet.rtc_time;

  packet.rtc_date.command = INT1A_RTCDATE_GET;
  if (call16 (bios_int1a, (uint16_t)(uint32_t)&packet, 0) != 0)
    {
      printf ("error.\n");
    }
  int year = BCD_TO_BIN (date->century) * 100 + BCD_TO_BIN (date->year);
  int month = BCD_TO_BIN (date->month);
  int day = BCD_TO_BIN (date->day);

  packet.rtc_date.command = INT1A_RTCTIME_GET;
  if (call16 (bios_int1a, (uint16_t)(uint32_t)&packet, 0) != 0)
    {
      printf ("error.\n");
    }
  int hour = BCD_TO_BIN (time->hour);
  int min = BCD_TO_BIN (time->min);
  int sec = BCD_TO_BIN (time->sec);

  printf ("%d/%d/%d %d:%d:%d\n", year, month, day, hour, min, sec);

  return 0;
}

uint32_t
bios_timer_get ()
{
  union bios_int1a_packet packet;

  packet.rtc_date.command = INT1A_CLOCK_GET;
  if (call16 (bios_int1a, (uint16_t)(uint32_t)&packet, 0) != 0)
    {
      return 0;
    }

  return packet.clock.cnt_high << 16 | packet.clock.cnt_low;
}

bool
bios_timer_set (uint32_t t)
{
  union bios_int1a_packet packet;

  packet.rtc_date.command = INT1A_CLOCK_SET;
  packet.clock.cnt_high = t >> 16;
  packet.clock.cnt_low = t;

  if (call16 (bios_int1a, (uint16_t)(uint32_t)&packet, 0) != 0)
    {
      return FALSE;
    }

  return TRUE;
}
