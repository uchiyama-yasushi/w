
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

// builtin command.
#include <sys/system.h>
#include <string.h>
#include <stdlib.h>

#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>

SHELL_COMMAND_DECL (reset);
SHELL_COMMAND_DECL (help);

void
shell_install_builtin_command ()
{

  // Install 'RESET' command. (Issue TRAPA with arg.)
  shell_command_register (&reset_cmd);
  // help command.
  shell_command_register (&help_cmd);
}

uint32_t
reset (int32_t argc, const char *argv[])
{
  int cause = 0;

  if (argc > 1)
    cause = atoi (argv[1]);

  board_reset (cause);

  return 0;
}
#ifndef THREAD_DISABLE
#include <sys/thread.h>
#ifndef MONITOR_DISABLE
#include <sys/monitor.h>
#endif
#endif

SLIST_HEAD (command_list, shell_command);
extern struct command_list command_list;

uint32_t
help (int32_t argc, const char  *argv[])
{
  struct shell_command *cmd;
  argc = argc, argv = argv;

#if defined DEBUG && !defined THREAD_DISABLE
#ifndef MONITOR_DISABLE
  monitor_debug ();
#endif
  thread_debug_stack_check ();
  thread_debug_state (0);
#endif
  printf ("avaliable command: ");
  SLIST_FOREACH (cmd, &command_list, command_link)
    {
      printf ("%s ", cmd->name);
    }
  printf ("\n");

  return 0;
}
