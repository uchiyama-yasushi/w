#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/fiber.h>
#include <sys/board.h>
#include <1655/timer.h>
#include <setjmp.h>
#include <string.h>

SHELL_COMMAND_DECL (setjmp_test);

void
board_main (uint32_t arg __attribute__((unused)))
{
  void fiber_test_setup (void);
  fiber_test_setup ();
  SHELL_COMMAND_REGISTER (setjmp_test);
  printf ("sizeof long int: %d\n", sizeof (long int));
  printf ("sizeof long long: %d\n", sizeof (long long));
  printf ("sizeof long: %d\n", sizeof (long));
  printf ("sizeof int: %d\n", sizeof (int));
  printf ("sizeof short: %d\n", sizeof (short));

  shell_prompt (stdin, stdout);
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{

  timer_init ();
}

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | DELAY_CALIBRATE;
}

void stack_check (const char *);

uint32_t
setjmp_test (int32_t argc __attribute__((unused)),
	     const char *argv[] __attribute__((unused)))
{
  jmp_buf env;
  int i;
  memset (env, 0, sizeof env);

  stack_check (0);
  if (!(i = setjmp (env)))
    {
      printf ("setjmp %x\n", env[3]);
    }
  else
    {
      printf ("longjmp (12345)%d\n", i);
      return 0;
    }
  longjmp (env, 12345);
  printf ("XXX longjmp failed.\n");

  return 0;
}
