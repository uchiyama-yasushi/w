
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

/* DO NOT ADD CODE. CAN NOT COMPILE WITH H8/300 */

#include <sys/system.h>
#include <sys/types.h>

#include <sys/console.h>
#include <sys/shell.h>
#include <sys/fiber.h>

#include <stdlib.h>

SHELL_COMMAND_DECL (fiber_test);
fiber_func fiber_1;
fiber_func fiber_2;
#ifndef FIBER_STACK_SIZE_DEFAULT
#define	FIBER_STACK_SIZE_DEFAULT	1024
#endif
fiber_t fiber[3];
uint8_t fls[2][FIBER_STACK_SIZE (FIBER_STACK_SIZE_DEFAULT)];

void fiber_test_setup (void);

void
fiber_test_setup ()
{

  shell_command_register (&fiber_test_cmd);
}

uint32_t
fiber_test (int32_t argc __attribute__((unused)), const char *argv[] __attribute__((unused)))
{
  struct fiber myself;
  uint32_t ret;

  fiber[0] = fiber_init (&myself);
  fiber[1] = fiber_create (&myself, fls[0], FIBER_STACK_SIZE_DEFAULT, fiber_1);
  fiber[2] = fiber_create (&myself, fls[1], FIBER_STACK_SIZE_DEFAULT, fiber_2);
  //  printf ("fiber storage=%d\n", FIBER_STACK_SIZE (FIBER_STACK_SIZE_DEFAULT));

  fiber_twist (&myself, 2, fiber[1], fiber[2]);

  ret = fiber_start (&myself, fiber[1]);
  printf ("r1= %ld (390)\n", ret);
  ret = fiber_start (&myself, fiber[2]);
  printf ("r2= %ld (44)\n", ret);
  ret = fiber_yield (&myself, 789);
  printf ("p: %ld (456)\n", ret);
  ret = fiber_yield (&myself, 999);
  printf ("p: %ld (456)\n", ret);

  return 0;
}

void FIBER_FUNC_ATTRIBUTE
fiber_1  (fiber_t myself)
{
  //  printf ("enter fiber_1 myself=0x%x parent=0x%x\n", myself, myself->parent);
  uint32_t ret = fiber_return_parent (myself, 390);

  //  printf ("%s start\n", __FUNCTION__);
  while (/*CONSTCOND*/1)
    {
      printf ("1: %ld (789/999)\n", ret);
      ret = fiber_yield (myself, 123);
    }
}

void FIBER_FUNC_ATTRIBUTE
fiber_2 (fiber_t myself)
{
  uint32_t ret = fiber_return_parent (myself, 44);

  //  printf ("%s start\n", __FUNCTION__);
  while (/*CONSTCOND*/1)
    {
      printf ("2: %ld (123)\n", ret);
      ret = fiber_yield (myself, 456);
    }
}
