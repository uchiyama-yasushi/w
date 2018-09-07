#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/board.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void *my_malloc (size_t);
void *my_realloc (void *, size_t);
void my_free (void *);

void
board_main (uint32_t arg __attribute__((unused)))
{
  int argc = 1;
  char *argv[] = { "lua_overlay", NULL };
  int main (int argc, char **argv);
  extern char bss_end[];

  sbrk_set_arena ((addr_t)bss_end, 0);
  console_polling ();
  printf ("ohayo0\n");
  main (argc, (char **)argv);
  while (/*CONSTCOND*/1)
    ;
}

void
stack_check (const char *str)
{
  str=str;
}
