
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
#include <stdarg.h>
#include <sys/console.h>
#define	FIBER_PRIVATE
#include <sys/fiber.h>


#define	FIBER_VERBOSE

#ifdef FIBER_VERBOSE
//#define	LPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#define	LPRINTF(fmt, args...)	iprintf(fmt, ##args)
#else
#define	LPRINTF(fmt, args...)	((void)0)
#endif

int __fiber_id;

__BEGIN_DECLS
uint32_t fiber_switch (fiber_t, fiber_t, uint32_t) FIBER_FUNC_ATTRIBUTE;
__END_DECLS

// make myself (thread) as fiber.
fiber_t
fiber_init (fiber_t f)
{
  f->id = ++__fiber_id;
  f->parent = NULL;

  return f;
}

// start new fiber from parent.
uint32_t
fiber_start (fiber_t myself, fiber_t start)
{

  return fiber_switch (myself, start, (uint32_t)(addr_t)start);
}

// corresponding start routine of child.
uint32_t
fiber_return_parent (fiber_t myself, uint32_t retarg)
{

  return fiber_switch (myself, myself->parent, retarg);
}

// continue to next fiber.
uint32_t
fiber_yield (fiber_t myself, uint32_t retarg)
{

  return fiber_switch (myself, myself->continuation, retarg);
}

// link continuation.
void
fiber_twist (fiber_t myself, int n, ...)
{
  va_list ap;
  fiber_t f, pre_f;
  int i;

  f = pre_f = NULL;
  va_start(ap, n);
  for (i = 0; i < n; i++)
    {
      f = va_arg (ap, fiber_t);
      if (pre_f)
	{
	  pre_f->continuation = f;
	  //	  f->caller = pre_f;
	  LPRINTF ("%ld->%ld\n", pre_f->id, f->id);
	}
      else
	{
	  myself->continuation = f;
	  LPRINTF ("[%ld->%ld\n", myself->id, f->id);
	  //	  f->caller = myself;
	}
      pre_f = f;
    }
  va_end (ap);
  f->continuation = myself;
  LPRINTF ("%ld->%ld]\n", f->id, myself->id);
}
