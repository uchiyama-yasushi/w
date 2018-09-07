
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

#ifndef _SHELL_H_
#define	_SHELL_H_
#include <sys/queue.h>
#include <stdio.h>

#ifndef SHELL_CMDARG_MAX
#define	SHELL_CMDARG_MAX	4
#endif

struct shell_command
{
  const char *name;
  uint32_t (*func)(int32_t, const char *[]);

  SLIST_ENTRY (shell_command) command_link;
};

// make sure to locate .data section not .rodata.
#define	SHELL_COMMAND_DECL(x)						\
  STATIC uint32_t x (int32_t, const char *[]);				\
 struct shell_command x ## _cmd __attribute ((section (".data")))=	\
  {									\
  name:  #x,								\
  func:  x,								\
  { NULL },								\
  }

#define	SHELL_COMMAND_REGISTER(x) shell_command_register (&  x ## _cmd)

__BEGIN_DECLS
extern char shell_command_buffer[];
void shell_init (void);
void shell_command_register (struct shell_command *);
uint32_t shell_command_exec (char *);
//void shell_set_device (enum console_device, enum console_device, bool);
void shell_prompt (struct _file *, struct _file *);
#ifndef SHELL_NOBUILTIN_COMMAND
void shell_install_builtin_command ();
#endif
__END_DECLS
#endif
