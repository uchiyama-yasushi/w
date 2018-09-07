/*	$NetBSD$	*/

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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");
#if defined _KERNEL_OPT
#include "opt_v7fs.h"
#endif

#ifdef _KERNEL
#include <sys/param.h>
#else
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#endif

#include "v7fs.h"
#include "v7fs_impl.h"

#if defined _KERNEL || defined __WOS__
#define	STATIC_BUFFER
#endif

int scratch_minfree = V7FS_SELF_NSCRATCH;

void *
scratch_read (struct v7fs_self *_, daddr_t blk)
{
#ifdef STATIC_BUFFER
  int i;
  MEM_LOCK (_);
  for (i = 0; i < V7FS_SELF_NSCRATCH; i++)
    {
      if (_->scratch_free & (1 << i))
	{
	  _->scratch_free &= ~(1 << i);
	  break;
	}
    }
  if (i == V7FS_SELF_NSCRATCH)
    {
      printf ("No scratch area. increase V7FS_SELF_NSCRATCH(>%d)\n",
	      V7FS_SELF_NSCRATCH);
      assert (0);
      MEM_UNLOCK (_);
      return NULL;
    }

  if (!_->io.read (_->io.cookie, _->scratch[i], blk))
    {
      printf ("*** %s: I/O error block %lx ***\n", __func__, (long)blk);
      //      assert (0);
      _->scratch_free |= (1 << i);
      MEM_UNLOCK (_);
      return NULL;
    }
  MEM_UNLOCK (_);
  // Statics
  int n;
  if ((n = scratch_remain (_)) < scratch_minfree)
    scratch_minfree = n;

  return _->scratch[i];
#else
  uint8_t *buf = malloc (V7FS_BSIZE);
  if (!_->io.read (_->io.cookie, buf, blk))
    {
      printf ("%s: I/O error block %lx\n", __func__, (long)blk);
      return NULL;
    }
  return buf;
#endif
}

int
scratch_remain (const struct v7fs_self *_)
{
#ifdef STATIC_BUFFER
  int nfree;
  int i;
  MEM_LOCK (_);
  for (i = 0, nfree = 0; i < V7FS_SELF_NSCRATCH; i++)
    {
      if (_->scratch_free & (1 << i))
	{
	  nfree++;
	}
    }
  MEM_UNLOCK (_);
  return nfree;
#else
  return -1;
#endif
}

void
scratch_free (struct v7fs_self *_ __attribute__((unused)), void *p)
{
#ifdef STATIC_BUFFER
  int i;
  MEM_LOCK (_);
  for (i = 0; i < V7FS_SELF_NSCRATCH; i++)
    if (_->scratch[i] == p)
      {
	_->scratch_free |= (1 << i);
	break;
      }
  MEM_UNLOCK (_);
  assert (i != V7FS_SELF_NSCRATCH);
#else
  free (p);
#endif
}
