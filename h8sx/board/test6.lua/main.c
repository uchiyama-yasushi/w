#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/fiber.h>
#include <sys/board.h>
#include <1655/timer.h>

SHELL_COMMAND_DECL (lua);

void
board_main (uint32_t arg __attribute__((unused)))
{
  SHELL_COMMAND_REGISTER (lua);

  shell_prompt (stdin, stdout);
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  //  timer_init ();
}

uint32_t
board_boot_config ()
{

  return RAM_CHECK | DELAY_CALIBRATE;
}

void
board_reset (uint32_t cause)
{
  cause=cause;
  while (/*CONSTCOND*/1)
    ;
  //NOTREACHED
}

uint32_t
lua (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  int main (int argc, char **argv);

  main (argc, (char **)argv);

  return 0;
}
