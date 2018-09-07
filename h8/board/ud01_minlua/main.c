#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/fiber.h>
#include <sys/board.h>
#include <unistd.h>

SHELL_COMMAND_DECL (luamin);

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
  SHELL_COMMAND_REGISTER (luamin);
  luamin (0,0);
}

uint32_t
board_boot_config ()
{

  return 0;
}

uint32_t
luamin (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  uint32_t r = 0;

  //  __asm volatile ("mov %0, sp" :: "r"(0x9dffc));
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  console_polling ();
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  clearerr (stdin);
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);
  printf ("%lx %lx %d %d %ld\n", 0x80000, 0x80000, 1024, 0,  (long)1);
  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);

  sbrk_set_arena (0x80000, 0);


  __asm volatile ("mov sp, %0" : "=r"(r));
  printf ("sp=%lx\n", r);

  main ();

  return 0;
}

void
stack_check (const char *str)
{
  printf ("%s\n", str);
}
