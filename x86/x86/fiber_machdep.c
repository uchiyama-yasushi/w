
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

#include <sys/system.h>
#include <sys/console.h>
#define	FIBER_PRIVATE
#include <sys/fiber.h>

#define	FIBER_VERBOSE

#ifdef FIBER_VERBOSE
#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

// create new fiber.
fiber_t
fiber_create (fiber_t parent, uint8_t *fiber_local_store, size_t stack_size,
	      fiber_func_t start)
{
  fiber_t f = (fiber_t)fiber_local_store;
  // Decrement return address pointer space.
  addr_t stack_bottom = (addr_t)(f->stack + stack_size - sizeof (addr_t));

  f->parent = parent;
  f->id = ++__fiber_id;

  f->regs.sp = (reg_t)stack_bottom; // np. fiber function is regparm(3).
  // install return address for 'ret'
  *(addr_t *)stack_bottom = (addr_t)start;

  LPRINTF ("%x %x %x\n", f->parent, f->regs.sp, *(addr_t *)stack_bottom);

  return f;
}