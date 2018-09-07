
/*-
 * Copyright (c) 2010 UCHIYAMA Yasushi.  All rights reserved.
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
#include <sys/shell.h>
#include <sys/board.h>
#include <sys/debug.h>
#include <stdlib.h>
#include <sys/thread.h>

SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (info);

#include "lua.h"
#include "lauxlib.h"

void stack_check (const char *);

static int
print (lua_State *L)
{
 int n = lua_gettop (L);
 int i;

 printf ("n=%d\n", n);

 for (i = 1; i <= n; i++)
   {
     if (i > 1)
       printf("\t");
     if (lua_isstring (L, i))
       printf("%s", lua_tostring (L ,i));
     else if (lua_isnil (L, i))
       printf("%s","nil");
     else if (lua_isboolean (L, i))
       printf("%s",lua_toboolean (L, i) ? "true" : "false");
     else
       printf("%s:%p", luaL_typename(L,i), lua_topointer(L,i));
   }
 printf("\n");

 return 0;
}

int
main ()
{
  stack_check ("lua start");
  lua_State *L = lua_open ();
  stack_check ("lua_open");
  lua_register (L, "print", print);
  stack_check ("lua_register");
  if (luaL_dofile (L, NULL) != 0)
    fprintf (stderr,"%s\n",lua_tostring (L, -1));
  stack_check ("luaL_dofile");
  lua_close(L);
  stack_check ("lua end");

  return 0;
}

void
board_main (uint32_t arg __attribute__((unused)))
{

  SHELL_COMMAND_REGISTER (test);
  SHELL_COMMAND_REGISTER (info);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

#include <cpu.h>

uint32_t
test (int32_t argc __attribute__((unused)),
   const char *argv[] __attribute__((unused)))
{
  clearerr (stdin);
  main ();
  return 0;
}


uint32_t
info (int32_t argc __attribute__((unused)),
      const char *argv[] __attribute__((unused)))
{

  stack_check (0);

  return 0;
}

void
end ()
{
}
