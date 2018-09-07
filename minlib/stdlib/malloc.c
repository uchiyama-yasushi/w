
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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

// V7 malloc.
#include <minlib.h>
#include <sys/types.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef STATIC
#define	STATIC
#endif

//#define	MALLOC_DEBUG
#ifdef MALLOC_DEBUG
#define	DPRINTF(fmt, args...)	printf (fmt, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

union store
{
  union store *next;
  intptr_t state;
#define	STORE_BUSY	1
  intptr_t value;
};

struct alloc
{
  union store *store_base;	// link bottom
  union store *store_hint;	// search starting point
  union store *store_top;	// link top
  intptr_t realloc_saved_value;	// for overlap
  int nmalloc;
  int nfree;
  int nrealloc;
  int pad;	// for align
};

#define	STORE_SIZE		sizeof (union store) // equals sizeof (intptr_t)
#define	BLOCK_SIZE		1024
#define	BLOCK_PER_STORE		(BLOCK_SIZE / STORE_SIZE)

#define	next(p)			((union store *)(((p)->state) & ~STORE_BUSY))
#define	busy(p)			((p)->state & STORE_BUSY)
#define	set_busy_block(p)	((p)->state |= STORE_BUSY)
#define	clear_busy_block(p)	((p)->state &= ~STORE_BUSY)
#define	set_link(p, q)		((p)->next = (q))
#define	payload_per_store(p)	(next (p) - (p) - 1)

#define	ROUND_STORE_COUNT(x)  (((x) + STORE_SIZE - 1) / STORE_SIZE)

#ifdef MALLOC_DEBUG
void store_sanity (struct alloc *, union store *);
#else
#define	store_sanity(p, q)	((void)0)
#endif

void *heap_allocate (struct alloc *, size_t);
void heap_deallocate (struct alloc *, void *);
void *heap_reallocate (struct alloc *, void *, size_t);

struct alloc *alloc_init (void *, size_t);
STATIC union store *store_search (struct alloc *, union store *, int);
STATIC int store_expand (struct alloc *, int, union store **, union store **);
STATIC void store_top_update (struct alloc *, union store *, union store *);
STATIC union store *store_adjust (struct alloc *, union store *, int);

static struct alloc *stdmem;
void *
LIBFUNC (malloc) (size_t nbytes)
{
  if (!stdmem)
    stdmem = alloc_init (sbrk (0), -1);

  return heap_allocate (stdmem, nbytes);
}

void
LIBFUNC (free) (void *ptr)
{

  heap_deallocate (stdmem, ptr);
}


void *
LIBFUNC (realloc) (void *ptr, size_t nbytes)
{
  if (!stdmem)
    stdmem = alloc_init (sbrk (0), -1);

  return heap_reallocate (stdmem, ptr, nbytes);
}

void *
heap_allocate (struct alloc *m, size_t nbytes)
{
  union store *p;
  int nstores;

  DPRINTF ("==========alloc %dbyte==========\n", nbytes);
  assert (m->store_base);
  /* Required store aligned size. */
  nstores = ROUND_STORE_COUNT (nbytes + sizeof (union store));

  store_sanity (m, 0);
  for (p = m->store_hint; !(p = store_search (m, p, nstores));)
    {
      union store *newtop;
      // Allocate new store
      if (!store_expand (m, nstores, &p, &newtop))
	return NULL;
      // Setup new link.
      store_top_update (m, p, newtop);
      // Search & merge from base.
      p = m->store_base;
      store_sanity (m, p);
    }
  // Store found.
  m->store_hint = store_adjust (m, p, nstores); // Next search point.
  // Mark this store is inuse.
  set_busy_block (p);

  store_sanity (m, p);
  DPRINTF ("==========alloc %p==========\n", p);

  m->nmalloc++;
  return (void *)(p + 1);
}

void
heap_deallocate (struct alloc *m, void *ptr)
{
  if (!ptr)
    return;

  union store *p = (union store *)ptr - 1;

  DPRINTF ("----------dealloc %p----------\n", p);
  store_sanity (m, p);
  // Mark this store is inuse.
  clear_busy_block (p);
  m->store_hint = p;	// Next alloc search from here.
  store_sanity (m, p);
  DPRINTF ("--------------------------\n");
  m->nfree++;
}

void *
heap_reallocate (struct alloc *m, void *ptr, size_t nbytes)
{
  union store *p = (union store *)ptr - 1;
  union store *q;
  union store *newptr;
  int neww;
  int oldw;

  if (!ptr)
    return heap_allocate (m, nbytes);
  m->nrealloc++;

  oldw = payload_per_store (p);
  neww = ROUND_STORE_COUNT (nbytes);

  heap_deallocate (m, ptr);
  // alloc search from last freeed store.
  if (!(newptr = heap_allocate (m, nbytes)) || newptr == ptr)
    return newptr;

  // Copy contents.
  int i, n = neww < oldw ? neww : oldw;
  for (i = 0; i < n; i++)
    newptr[i] = ((union store *)ptr)[i];

  q = (union store *)newptr - 1;

  /* Treat overlap */
  if ((q < p) &&
          (p <= q + neww))
    {
      /* Restore value.
                           +overrided by union store.
                 p         | p->next
    |    p-q     |         *   |
               +---(p-q)---+
    |          *           |
    q                   q->next

    pointer of restore: q->next - (p - q)
      */
      (next (q) - (p - q))->value = m->realloc_saved_value;
      DPRINTF ("restore point:%p\n", next (q) - (p - q));
    }

  return newptr;
}

struct alloc *
alloc_init (void *start, size_t size __attribute__((unused)))
{

  brk (start);
  struct alloc *m = (struct alloc *)
    sbrk (sizeof (union store) * 2 + sizeof (struct alloc));
  memset (m, 0, sizeof (struct alloc));

  m->store_base = (union store *)(m + 1);

  DPRINTF ("START %p %p\n", m->store_base, m->store_base + 1);
  /* link each other */
  set_link (m->store_base, m->store_base + 1);
  set_link (m->store_base + 1, m->store_base);
  /* base and top is never used for store. */
  set_busy_block (m->store_base);
  set_busy_block (m->store_base + 1);

  m->store_top = m->store_base + 1;
  m->store_hint = m->store_base;

  return m;
}

union store *
store_search (struct alloc *m __attribute__((unused)), union store *p,
	      int nstores)
{
  int i;

  // 2pass for loop over all stores.
  for (i = 0; i < 2; p = next (p))
    {
      // If current block is free, Merge free blocks. If enuf area
      // is found, use this.
      if (!busy (p))
	{
	  union store *q;
	  for (q = next (p); !busy (q); q = next (p))
	    {
	      assert (q > p); // Don't loop over.
	      assert (q < m->store_top); // store_top is always busy.
	      DPRINTF ("merge %p %p\n", p, q);
	      p->next = q->next; // merge block.
	    }
	  if (q >= p + nstores) // Is this store has enuf region?
	    {
	      DPRINTF ("found! %p\n", p);
	      return p;
	    }
	}
      if (next (p) < p) // loop over.
	{
	  assert (next (p) == m->store_base);
	  i++;
	}
    }
  assert (p == m->store_base);

  return 0;
}

int
store_expand (struct alloc *m __attribute__((unused)), int nstores,
	      union store **newstore, union store **top)
{
  // Round to block alignment
  int nblocks = ((nstores + BLOCK_PER_STORE) / BLOCK_PER_STORE) *
    BLOCK_PER_STORE;
  size_t len = nblocks * STORE_SIZE;
  union store *p;

  // Get new bucket from heap
  if ((intptr_t)(p = (union store *)sbrk (len)) < 0)
    {
      DPRINTF ("sbrk failed\n");
      return 0;
    }
  DPRINTF ("sbrk %p %dbyte\n", p, len);
  assert (p > m->store_top);
  *newstore = p;
  *top = p + nblocks - 1;

  return 1;
}

void
store_top_update (struct alloc *m, union store *newstore, union store *top)
{
  union store *oldtop = m->store_top;
  m->store_top = top;

  set_link (oldtop, newstore); // link to newly allocated area. clear busy.
  if (newstore != oldtop + 1) // has gap previous sbrk.
    {
      /*
sbrk continuous
  store_top
    ||
     q
Ok to merge store_top and q

sbrk non-continuous
  store_top
    |
             |
             q
Can't merge store_top and q. so set busy flag.
      */
      DPRINTF ("sbrk non-continuous");
      set_busy_block (oldtop); // Stop marker. This store can't merge.
    }
  set_link (newstore, m->store_top);
  set_link (m->store_top, m->store_base); // loop.
  set_busy_block (m->store_top);	// alloc top is always busy.

  DPRINTF ("%p, %p\n", newstore, oldtop +1);
}

union store *
store_adjust (struct alloc *m, union store *newstore, int nstores)
{
  union store *p;

  assert (newstore->next >= newstore + nstores);

  /*
    |                          |            |
    newstore                +nstores     newstore->next
  */
  if (newstore->next != (p = newstore + nstores))
    {
      // If this store is bigger than request, split store.
      // When relloced area overlapped old area, this union store
      // overwrite previous buffer. To restore after, save value.
      m->realloc_saved_value = p->value; // old value. restored by realloc.
      DPRINTF ("saved value=%x\n", m->realloc_saved_value);
      set_link (p, newstore->next);
      set_link (newstore, p);
    }

  assert (p <= m->store_top);

  return p;
}

#ifdef MALLOC_DEBUG
void
store_sanity_check (struct alloc *m, union store *p)
{
  if (p == m->store_top || p == m->store_base)
    {
      assert (busy (p));
    }
  else
    {
      assert (p < m->store_top);
      assert (p > m->store_base);
    }

  if (next (p) > p)
    assert (next (p) <= m->store_top);
  else
    assert (next (p) == m->store_base);
}

int
store_type (struct alloc *m, union store *p, union store *q, int *start)
{

  if (p == m->store_hint)
    {
      (*start)++;
      return 0;
    }

  return p == q ? 1 : 2;
}

void
store_sanity (struct alloc *m, union store *s)
{
  char paren[][2] = { { '[', ']' }, { '{', '}' }, { ' ', ' ' }};
  char *parenp;
  union store *p;
  int i, start, busy;

  printf ("CHAIN: ");
  for (i = start = busy = 0, p = m->store_base; next (p) > p; p = next (p), i++)
    {
      store_sanity_check (m, p);
      if (busy (p))
	busy++;
      parenp = paren[store_type (m, p, s, &start)];
#if 0
      printf ("%c%p%c(%d)%c-", parenp[0], p, busy (p) ? '*' : '_',
	      payload_per_store (p) * STORE_SIZE, parenp[1]);
#endif
    }
  parenp = paren[store_type (m, p, s, &start)];
  i++;
  printf ("%c%p%c(%d)%c\n", parenp[0], p, busy (p) ? '*' : '_',
	  payload_per_store (p) * STORE_SIZE, parenp[1]);
  printf ("link:%d busy:%d free:%d malloc:%d free:%d realloc:%d\n",
	  i, busy, i - busy, m->nmalloc, m->nfree, m->nrealloc);
  assert (p == m->store_top);
  assert (next (p) == m->store_base);
  assert (start == 1);
}
#endif //MALLOC_DEBUG
#if 0
int
main ()
{
#define	TEST_BYTES	300
#define	TEST_BYTES1	400
  int i;
  struct alloc *m = alloc_init (0x700000, 0x100000);

  void *p0 = my_malloc (500);
  memset (p0, 0x5a, 500);
  void *p1 = my_malloc (500);
  memset (p1, 0xa5, 500);
  my_free (p0);
  p0 = my_realloc (p1, 700);

  char *p;
  for (p = (char *)p0, i = 0; i < 500; i++, p++)
    {
      printf ("%d %x\n",i, *p & 0xff);
      //      assert ((*p & 0xff) == 0xa5);
    }

  for (i = 0; i < 2; i++)
    {
      char *p0 = (char *)my_realloc (0, TEST_BYTES);
      printf ("[%p]\n", p0);
      memset (p0, 0xff, TEST_BYTES);
      sbrk (1024);

      char *p1 = (char *)my_realloc (0, TEST_BYTES1);
      printf ("[%p]\n", p1);
      memset (p1, 0xff, TEST_BYTES1);
      my_free (p0);
      my_free (p1);
    }
  return 0;
}
#endif
