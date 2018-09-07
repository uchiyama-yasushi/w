
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

// Akizuki GP2Y0A21 YK0F
/*
  Distance Measuring Sensor Unit
  Measuring distance: 10 to 80 cm
  Analog output type
 */
#include <sys/system.h>
#include <sys/console.h>
#include <sys/delay.h>

#include <cpu.h>
#include <reg.h>
#include "distancemeter.h"
#include "local.h"

#define	ADC_CH1		1

void
distancemeter_init ()
{

  adc_init ();
  adc_channel_enable (ADC_CH1);
}

bool
distancemeter_read (uint32_t *v)
{

  return adc_conversion (ADC_CH1, v);
}

// For debug
void
postprocess_distancemeter (uint32_t event __attribute__((unused)),
			   int32_t *buf __attribute__((unused)))
{
#ifdef DEBUG
  iprintf ("distance:%d\n", *buf);
#endif
}
