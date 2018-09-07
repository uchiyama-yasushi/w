
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

// General Purpose Input/Output port

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

STATIC uint32_t __fio[] = { FIO0Base, FIO1Base, FIO2Base, FIO3Base, FIO4Base };

void
gpio_dir (int port, int pin, enum gpio_dir gpio_dir)
{
  volatile uint32_t *dir = (volatile uint32_t *)(__fio[port] + FIODIR);

  if (gpio_dir == GPIO_INPUT)
    *dir &= ~(1 << pin);
  else
    *dir |= (1 << pin);
}

void
gpio_interrupt (int port, int pin, int edge)
{
  assert ((port == 0) || (port == 2));
  volatile uint32_t *rising_edge = port ? IO2IntEnR : IO0IntEnR;
  volatile uint32_t *falling_edge = port ? IO2IntEnF : IO0IntEnF;
  uint32_t bit = (1 << pin);

  *falling_edge &= ~bit;
  *rising_edge &= ~bit;

  if (edge & GPIO_EDGE_F)
    {
      *falling_edge |= bit;
    }
  if (edge & GPIO_EDGE_R)
    {
      *rising_edge |= bit;
    }
}
