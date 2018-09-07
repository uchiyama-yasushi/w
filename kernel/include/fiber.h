
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

#ifndef _FIBER_H_
#define	_FIBER_H_
#include <sys/types.h>
#include <machine/fiber.h>

struct fiber;
typedef struct fiber *fiber_t;
typedef void (fiber_func)(fiber_t) FIBER_FUNC_ATTRIBUTE;
typedef void (*fiber_func_t)(fiber_t) FIBER_FUNC_ATTRIBUTE;

struct fiber
{
  struct fiber_reg regs;// machine dependent.
  fiber_t parent;	// normal-mode:2byte, advanced-mode 4byte (H8)
  fiber_t continuation;	// normal-mode:2byte, advanced-mode 4byte (H8)
  int32_t id;
  uint8_t stack[0];
} __attribute__((packed, aligned (4)));

#define	FIBER_STACK_SIZE(x)	(ROUND ((sizeof (struct fiber) + (x)), 4))

#ifdef FIBER_PRIVATE
extern int __fiber_id;
#endif

__BEGIN_DECLS
fiber_t fiber_init (fiber_t);
fiber_t fiber_create (fiber_t, uint8_t *, size_t, fiber_func_t);
void fiber_twist (fiber_t, int/*# of child*/, ...);
uint32_t fiber_start (fiber_t, fiber_t);
uint32_t fiber_return_parent (fiber_t, uint32_t);
uint32_t fiber_yield (fiber_t, uint32_t);
__END_DECLS

#endif
