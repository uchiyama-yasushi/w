
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

// Thread stack allocator.
#ifndef _THREAD_STACK_H_
#define	_THREAD_STACK_H_
#include <sys/queue.h>

#ifndef	THREAD_STACK_POOL_MAX
#define	THREAD_STACK_POOL_MAX		THREAD_PRIORITY_MAX
#endif
#define	THREAD_STACK_MAGIC		0xaa

#ifndef THREAD_STACKSIZE_DEFAULT
#if defined __arm__
#define	THREAD_STACKSIZE_DEFAULT	1024
#elif defined __sh__
#define	THREAD_STACKSIZE_DEFAULT	1024
#elif defined __H8300H__
#ifdef __NORMAL_MODE__	// Tiny
#define	THREAD_STACKSIZE_DEFAULT	256
#else	// Advanced mode.
#define	THREAD_STACKSIZE_DEFAULT	512
#endif
#elif defined __H8300SX__
#define	THREAD_STACKSIZE_DEFAULT	1024
#elif defined __i386__
#define	THREAD_STACKSIZE_DEFAULT	1024
#else
#error "unknown architecture"
#endif
#endif

// For dedicated thread stack.
#define	THREAD_STACK_SIZE(x) (ROUND ((sizeof (struct thread_control) + (x)), 4))

struct thread_stack
{
  uint8_t *bottom;
  uint8_t *top;
  size_t size;
  size_t remain;
  bool active;	// Thread uses this stack.
  SIMPLEQ_ENTRY (thread_stack) ts_link;
};
SIMPLEQ_HEAD (thread_stack_pool, thread_stack);
extern struct thread_stack_pool thread_stack_pool;
extern int thread_stack_loaded;

__BEGIN_DECLS
// Initialize stack pool.
void thread_stack_init (void);
// Convert address space to canonical stack stucture.
void thread_stack_setup (struct thread_stack *, uint8_t *, size_t);
// Load stack to pool.
void thread_stack_load (struct thread_stack *);
// Convert non-detachable stack region to canonical fashion.
void thread_stack_dedicate (uint8_t *, size_t);
// Handoff detachable stack space.
void thread_stack_handoff (thread_t, thread_t);
// Allocate stack from pool.
struct thread_stack *thread_stack_allocate (void);
// Return stack to pool.
void thread_stack_deallocate (struct thread_stack *);
// Attach newly allocated stack to thread with continuation.
void thread_stack_attach (thread_t, struct thread_stack *, continuation_func_t);
// Detach thread stack
void thread_stack_detach (thread_t);
#ifdef DEBUG
void thread_stack_debug_state (void);
bool thread_stack_debug_count_remain (struct thread_stack *);
#endif
__END_DECLS
#endif
