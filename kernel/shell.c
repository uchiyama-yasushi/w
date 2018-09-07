
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

// Simple command launcher.
#include <sys/system.h>
#include <string.h>
#include <stdlib.h>
#ifndef THREAD_DISABLE
#include <sys/thread.h>
#endif
#include <stdio.h>
#include <sys/prompt.h>
#include <sys/shell.h>

STATIC int8_t (*__getc) (void *, continuation_func_t);
STATIC void *__getc_arg;
char shell_command_buffer[PROMPT_BUFFER_SIZE];
STATIC char *argp[SHELL_CMDARG_MAX];

SLIST_HEAD (command_list, shell_command) command_list;
continuation_func shell_prompt_main __attribute__((noreturn));

void
shell_init ()
{

  SLIST_INIT (&command_list);
}

void
shell_command_register (struct shell_command *cmd)
{

  SLIST_INSERT_HEAD (&command_list, cmd, command_link);
}

void
shell_prompt (struct _file *in, struct _file *out)
{
#ifndef SHELL_NOBUILTIN_COMMAND
  shell_install_builtin_command (); // help, reset
#endif
#ifndef THREAD_DISABLE
  thread_priority (current_thread, PRI_LOW);	// lowest.
#endif

  prompt_init (out);
  __getc = in->char_ops->getc_cont;
  __getc_arg = in->char_ops->ctx;

  shell_prompt_main ();
  /* NOTREACHED */
}

void
shell_prompt_main ()	// continuation start.
{
  intr_enable ();
  int8_t *b;

  while (/*CONSTCOND*/1)
    {
      // Make this thread stack discardable.
      if ((b = prompt_input (__getc (__getc_arg, shell_prompt_main)))
	  == NULL)
	continue;
      memset (shell_command_buffer, 0, PROMPT_BUFFER_SIZE);
      strncpy ((char *)shell_command_buffer, (const char *)b,
	       PROMPT_BUFFER_SIZE);
      shell_command_exec (shell_command_buffer);
      prompt_reset ();
    }
  //NOTREACHED
}

uint32_t
shell_command_exec (char *buf)
{
  struct shell_command *cmd;
  const char *p0, *q0;
  char *p, *q;
  int i, argc, sep;

  p = q = 0;
  if (!*buf)
    return 0;

  SLIST_FOREACH (cmd, &command_list, command_link)
    {
      /* command name match */
      for (q0 = buf, p0 = cmd->name; *p0; p0++, q0++)
	{
	  if (*p0 != *q0)
	    break;
	}
      if (*p0 == 0 && (*q0 == '\0' || *q0 == ' '))
	goto found;
    }
  return -1;
 found:
  /* setup argument */
  p = buf;
  argc = 0;
  argp[argc++] = p;
  sep = 0;
  for (i = 0; (i < PROMPT_BUFFER_SIZE) && (argc <= SHELL_CMDARG_MAX); i++, p++)
    {
      if (*p == ' ')
	{
	  *p = 0;
	  sep = 1;
	}
      else if (sep && (*p != ' ' || *p == '\0'))
	{
	  sep = 0;
	  argp[argc++] = p;
	}
    }
  *--p = 0;
  return cmd->func (argc, (const char **)argp);
}
