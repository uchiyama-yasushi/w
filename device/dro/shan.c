
/*-
 * Copyright (c) 2008-2009 UCHIYAMA Yasushi.  All rights reserved.
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

/* SHAN digital caliper */
// This module is H8 port configuration independent.

#include <sys/system.h>
#include <sys/thread.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <string.h>

#include <sys/delay.h>

#include "caliper_manager.h"
#include "caliper_common.h"
#include "shan.h"

// interrupt handler common data.
int32_t shan_data[SHAN_AXIS_MAX];
uint8_t shan_data_updated;
uint8_t shan_error;
/*
  7     Axis3 sampling error
  6	Axis3 frame error
  5     Axis2 sampling error
  4	Axis2 frame error
  3     Axis1 sampling error
  2	Axis1 frame error
  1     Axis0 sampling error
  0	Axis0 frame error
 */
uint8_t shan_error_cause;

STATIC bool shan_update (caliper_t);
STATIC int32_t shan_data_format (int32_t);

STATIC struct caliper_ops shan_ops =
  {
    shan_init, shan_update,
    shan_command_power, shan_command_fast, shan_command_reset
  };

caliper_t
shan_attach (struct shan_caliper *sc, int axis, struct caliper_bits *bits)
{
  assert (axis < SHAN_AXIS_MAX);

  sc->caliper.ops = shan_ops;
  sc->caliper.bits = *bits;
  sc->caliper.update_bit = 1 << axis;
  sc->caliper.fast_mode = FALSE;
  sc->irq_handler_data = shan_data + axis;

  return (caliper_t)sc;
}

bool
shan_update (caliper_t caliper)
{
  struct shan_caliper *sc = (struct shan_caliper *)caliper;
  int32_t data;

  if (!sc->caliper.power)
    return FALSE;

  if (!(shan_data_updated & caliper->update_bit))
    {
      return FALSE;
    }
  data = *sc->irq_handler_data;

  shan_data_updated &= ~caliper->update_bit;
  shan_error = 0;

  caliper->raw_data = shan_data_format (data);

  return TRUE;
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
