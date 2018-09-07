
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

#ifndef _CALIPER_MANAGER_H_
#define	_CALIPER_MANAGER_H_
#include <sys/thread.h>
#include "caliper_config_limits.h"

struct caliper;

typedef struct caliper *caliper_t;

struct caliper_bits
{
  uint8_t data;
  uint8_t clock;
  uint8_t power;
  uint8_t irq;
};

struct caliper_ops
{
  void (*init) (caliper_t);
  bool (*update) (caliper_t);
  void (*power) (caliper_t);
  void (*fast) (caliper_t);
  void (*reset) (caliper_t);
};

struct caliper
{
  struct caliper_ops ops;
  struct caliper_bits bits;
  uint8_t update_bit;
  uint8_t axis;
  uint8_t fast_mode;
  uint8_t power;

  volatile uint8_t *pullup_port;
  volatile uint8_t *power_port;

  int32_t raw_data;	// caliper data.
};

#define	BLANK_CHARACTER	':'	// '0' + 10

__BEGIN_DECLS
void caliper_manager (uint32_t);
void caliper_manager_power (int);
void caliper_manager_fast (int);
void caliper_manager_reset (int);
extern thread_t caliper_manager_thread; // for sync.
__END_DECLS
#endif
