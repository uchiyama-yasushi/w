#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <sys/fiber.h>
#ifndef OVERLAY
#include <sys/board.h>
#endif

void
board_main (uint32_t arg __attribute__((unused)))
{

  void fiber_test_setup (void);
  fiber_test_setup ();

  printf ("%s\n", __FUNCTION__);
#ifndef OVERLAY
  shell_prompt (stdin, stdout);
#endif
}

#ifndef OVERLAY
uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | BUFFERED_CONSOLE_ENABLE | DELAY_CALIBRATE;
}

void
null_device_init (uint32_t arg __attribute__((unused)))
{
}
void board_device_init (uint32_t)
     __attribute ((weak, alias ("null_device_init")));
#endif
