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
#endif

#include "v7fs.h"
#include "v7fs_impl.h"
#include "v7fs_endian.h"
#include "v7fs_superblock.h"
#include "v7fs_inode.h"

int
v7fs_core_init (struct v7fs_self **p, const struct v7fs_mount_device *mount)
{
  int error;

  // Setup I/O ops.
  if ((error = v7fs_os_init (p, mount, V7FS_BSIZE)))
    {
      printf ("I/O setup failed.\n");
      return error;
    }
  struct v7fs_self *_ = *p;
  _->scratch_free = -1;
#ifdef V7FS_EI
  v7fs_endian_init (_, mount->endian);
#else
  if (BYTE_ORDER != mount->endian)
    {
      printf ("Recompile with `option V7FS_EI'");
      v7fs_os_fini (_);
      return EINVAL;
    }
#endif

  // Setup super block.
  if ((error = v7fs_superblock_load (_)))
  {
    if (error == EIO)
      printf ("Can't read superblock\n");
    else
      printf ("incorrect superblock.\n");

    return error;
  }

  return 0;
}

void
v7fs_core_fini (struct v7fs_self *_)
{

  v7fs_os_fini (_);
}
