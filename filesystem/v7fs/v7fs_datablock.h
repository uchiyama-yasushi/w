
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

#ifndef _V7FS_DATABLOCK_H_
#define	_V7FS_DATABLOCK_H_

__BEGIN_DECLS
bool datablock_number_sanity (const struct v7fs_self *, v7fs_daddr_t);
int v7fs_datablock_allocate (struct v7fs_self *, v7fs_daddr_t *);
int v7fs_datablock_foreach
(struct v7fs_self *, struct v7fs_inode *,
 int (*) (struct v7fs_self *, void *, v7fs_daddr_t, size_t), void *);
v7fs_daddr_t v7fs_datablock_last (struct v7fs_self *, struct v7fs_inode *,
    v7fs_off_t);
int v7fs_datablock_expand (struct v7fs_self *, struct v7fs_inode *, size_t);
int v7fs_datablock_contract (struct v7fs_self *, struct v7fs_inode *, size_t);
__END_DECLS
#endif //!_V7FS_INODE_H_
