#include <sys/system.h>
#include <sys/console.h>
#include <sys/shell.h>
#include <cpu.h>
#include <cache.h>
#include <mmu.h>
#include <sys/board.h>	// board dependent initialize hooks.

#include <reg.h>
#include <board/renesas_7785lcr/reg.h>
#include <board/renesas_7785lcr/spec.h>

#include <string.h>
#include <stdlib.h>

#include <pte.h>
#include <vm.h>
#include <fpool.h>

#include <sys/delay.h>
#include <sys/timer.h>

SHELL_COMMAND_DECL (timer);
SHELL_COMMAND_DECL (test);
SHELL_COMMAND_DECL (exception);
SHELL_COMMAND_DECL (cache);
SHELL_COMMAND_DECL (pmb);
SHELL_COMMAND_DECL (mem);
SHELL_COMMAND_DECL (memtest);
SHELL_COMMAND_DECL (cacheflush);
SHELL_COMMAND_DECL (tlbdump);
SHELL_COMMAND_DECL (tlbclear);
SHELL_COMMAND_DECL (mmu);
SHELL_COMMAND_DECL (vm);

//XXX
int __fpscr_values[2];

// R0P7785LC0011RL internal device.
uint8_t dip_switch (void);
void led (uint8_t);
void pmb_setup (void);

// Test thread.
void test_thread (uint32_t);
struct thread_control test_tc;
thread_t test_th;

uint32_t
board_boot_config ()
{

  return CONSOLE_ENABLE | RAM_CHECK | BUFFERED_CONSOLE_ENABLE |
    DELAY_CALIBRATE;
}

void
board_device_init (uint32_t arg __attribute__((unused)))
{
  cpu_init (SH7785LCR_INPUT_CLOCK, 16);
  cpu_info ();

  sh4a_cache_disable ();	// Write-back all.
  pmb_setup ();
  sh4a_cache_enable (TRUE);	// Enbale cache again.

  timer_init ();
}

void
board_main (uint32_t arg)
{
  printf ("%s: arg=%x\n",__FUNCTION__, arg);
  thread_t th;

  shell_command_register (&test_cmd);
  shell_command_register (&exception_cmd);
  shell_command_register (&timer_cmd);
  shell_command_register (&cache_cmd);
  shell_command_register (&pmb_cmd);
  shell_command_register (&mem_cmd);
  shell_command_register (&memtest_cmd);
  shell_command_register (&cacheflush_cmd);
  shell_command_register (&tlbdump_cmd);
  shell_command_register (&tlbclear_cmd);
  shell_command_register (&mmu_cmd);
  shell_command_register (&vm_cmd);

  th = thread_create_no_stack (&test_tc, "test", test_thread, 0x1234abcd);
  thread_start (th);

  test_th = th;
  cpu_status_t s = intr_suspend ();
  cpg_dump ();
  intr_resume (s);

  shell_prompt (stdin, stdout);
  // NOTREACHED
}

void
board_reset (uint32_t cause __attribute__((unused)))
{
  // Shutdown board.
  //  *POFCR = 1;

  // Reset board pripherals.
  //  *LOCALCR = 1;
  //XXX Delay 10us
  cpu_reset ();
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}

void
pmb_setup ()
{
  struct
  {
    uint32_t aa;
    uint32_t da;
  } pmb [] = {
#if 0
    // P1
    { 0x80000000 | PMB_AA_V,
      0x00000000 | PMB_DA_V | PMB_DA_C | PMB_DA_SZ_512M },
    // P2
    { 0xa0000000 | PMB_AA_V,
      0x00000000 | PMB_DA_V | PMB_DA_SZ_512M },
#else
    // P1
    { 0x89000000 | PMB_AA_V,
      0x09000000 | PMB_DA_V | PMB_DA_C | PMB_DA_SZ_16M },
    // P2
    { 0xa9000000 | PMB_AA_V,
      0x09000000 | PMB_DA_V | PMB_DA_SZ_16M },
    // PLD
    { 0xa4000000 | PMB_AA_V,
      0x04000000 | PMB_DA_V | PMB_DA_SZ_16M },
    // RAM
    //    { 0x90000000 | PMB_AA_V,
    //      0x40000000 | PMB_AA_V | PMB_DA_C | PMB_DA_SZ_128M },
    //    { 0x90000000 | PMB_AA_V,
    //      0x48000000 | PMB_AA_V | PMB_DA_C | PMB_DA_SZ_128M },//memtest failed.
    //    { 0x90000000 | PMB_AA_V,
    //      0x50000000 | PMB_AA_V | PMB_DA_C | PMB_DA_SZ_128M },
    { 0x90000000 | PMB_AA_V,
      0x58000000 | PMB_AA_V | PMB_DA_C | PMB_DA_SZ_128M },
#endif
  };
  // Change PMB setting P1/P2
  sh4a_pmb_entry_set_self (pmb[0].aa, pmb[0].da, pmb[1].aa, pmb[1].da);

  // Additional PMB.
  size_t i;
  for (i = 2; i < sizeof pmb / sizeof (pmb[0]); i++)
    {
      if (!sh4a_pmb_align_check (pmb[i].aa, pmb[i].da))
	continue;
      sh4a_pmb_entry_set (i, pmb[i].aa, pmb[i].da);
      *SH4A_MMUCR |= MMUCR_TI;	// Invalidate TLB
      CPU_SYNC_RESOURCE ();	// For MMUCR
    }
}

continuation_func test_main __attribute__((noreturn));

void
test_thread (uint32_t arg)
{
  printf ("%s:%x %d\n", __FUNCTION__, arg, sizeof (struct thread_control));

  led (dip_switch ());

  intr_enable ();
  int *p = (int *)current_thread->scratch;
  *p = 0;
  test_main ();
  // NOTREACHED
}

void
test_main ()
{
  int *p = (int *)current_thread->scratch;
  printf ("hello %d\n", (*p)++);
  thread_block (test_main);
  // NOTREACHED
  while (/*CONSTCOND*/1)
    ;
}

uint8_t
dip_switch ()
{

  return *SWSR;
}

void
led (uint8_t sw)
{

  int i;

  for (i = 1; i < 256; i <<= 1)
    {
      uint8_t r = i | i << 4;
      if (sw & i)
	*LEDCR |= r;
      else
	*LEDCR &= ~r;
    }
}

uint32_t
exception (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  // __asm volatile (".short 0xfffd");	// 0x180
  // __asm volatile ("trapa #33");	// 0x160
  //  __asm volatile ("1: nop; bra 1b; mov.l 2f, r0;.align 2;2: .long 0xaa");//0x1a0
  //  __asm volatile (".byte 0xff"); // 0x100
  //  __asm volatile ("mov r15, r0;add #1, r0;mov.l @r0, r1");// 0xe0
  //  __asm volatile ("mov r15, r0;add #1, r0;mov.l r1, @r0"); //0x100
  //  __asm volatile ("fneg fr0");	// 0x800
  //  __asm volatile ("bra 1f;fneg fr0;1:");// 0x820
  extern void fpu_exception_init (void);
  uint32_t r;
  __asm volatile ("sts	fpscr, %0" : "=r" (r));
  printf ("%x\n", r);

  fpu_exception_init ();

  __asm volatile ("sts	fpscr, %0" : "=r" (r));
  printf ("%x\n", r);

  float i = 0.;
  __asm volatile ("fmov.s %0, fr0;fdiv fr0, fr1":: "m"(i)); // 0x120
  printf ("skip instruction\n");

  return 0;
}

uint32_t
cacheflush (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  sh4a_dcache_wbinv_all ();

  return 0;
}

uint32_t
mem (int32_t argc, const char *argv[])
{
  if (argc < 3)
    return 0;

  uint32_t addr = strtol (argv[2], NULL, 16);
  uint32_t val = 0;

  switch (*argv[1])
    {
    case 'r':
      val = *(volatile uint32_t *)addr;
      iprintf ("(r)%x: %x\n", addr, val);
      break;
    case 'w':
      if (argc < 4)
	return 0;
      val = (uint32_t)strtol (argv[3], NULL, 16);
      iprintf ("(w)%x: %x\n", addr, val);
      *(volatile uint32_t *)addr = val;
      break;
    case 'c':
      if (argc < 4)
	return 0;
      uint32_t size = (uint32_t)strtol (argv[3], NULL, 16);
      iprintf ("clear %x-%x\n", addr, addr + size);
      memset ((void *)addr, 0, size);
      break;

    case 'f':
      val = (uint32_t)strtol (argv[2], NULL, 16);
      uint32_t a;
      uint32_t start = 0x0;
      uint32_t end = 0x20000000;

      iprintf ("Find pattern %x... (%x-%x)\n", val, start, end);
      for (a = start; a < end; a+= 4)
	{
	  if (*(volatile uint32_t *)a == val)
	    {
	      iprintf ("%x found at %x\n", val, a);
	    }
	}
    }

  return 0;
}

uint32_t
memtest (int32_t argc, const char *argv[])
{
  if (argc < 3)
    return 0;

  uint32_t addr = strtol (argv[1], NULL, 16);
  uint32_t size = strtol (argv[2], NULL, 16);

  check_ram (addr, addr + size, TRUE);

  return 0;
}

uint32_t
pmb (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

#ifdef SH4A_EXT_ADDR32
  sh4a_pmb_dump ();
#endif

  return 0;
}

uint32_t
mmu (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
#ifdef SH4A_EXT_MMU
  struct tlb_entry tlb_entry =
    { 0x00000000, 0x40000000, 0, WRITEBACK, PG_64M, P_RW, TRUE };

  sh4a_tlb_clear_all ();

  sh4a_mmu_start ();

  int i;
  for (i = 0; i < 8; i++)
    {
      sh4a_tlb_wired_entry (&tlb_entry);
      tlb_entry.vpn += 0x04000000;
      tlb_entry.ppn += 0x04000000;
    }
#endif
  return 0;
}

uint32_t
vm (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  sh4a_tlb_clear_all ();
  sh4a_mmu_start ();

  fpool_load_memory (0x5c000000, 0x4000000);
  return 0;
}

uint32_t
tlbdump (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  sh4a_mmu_dump ();

  return 0;
}

uint32_t
tlbclear (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  sh4a_tlb_clear_all ();

  return 0;
}

uint32_t
test (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{
  uint32_t r;

  CPU_GET_SR (r);
  cpu_dump_sr (r);
#if 0
  sh4a_dcache_wbinv_range_index (0x80000000, 0x100);
  sh4a_dcache_wbinv_range (0x80001000, 0x100);
  sh4a_dcache_wb_range (0x80002000, 0x100);
  sh4a_dcache_inv_range (0x80003000, 0x100);
  sh4a_dcache_wbinv_all ();
#endif
  r = cpu_get_pc ();
  printf ("PC=%x\n", r);
  cpu_run_P2 ();
  r = cpu_get_pc ();
  printf ("PC=%x\n", r);
  cpu_run_P1 ();
  r = cpu_get_pc ();
  printf ("PC=%x\n", r);
#if 0
  sh7785_icache_sync_range (0x80004000, 0x200);
  sh7785_icache_sync_range_index (0x80005000, 0x200);
  sh7785_icache_sync_all ();
#endif
  printf ("%x %x %x %x %x\n",*TCNT0, *TCR0, *INT2B0, *INT2A0, *INT2A1);

  int i = 0;
  while (1)
    {
      mdelay (1000);
      iprintf ("%d\n", i++);
    }

  return 0;
}

uint8_t test_buf[0x2000 + 64] __attribute__((aligned (32)));

void
dump_from_p2 ()
{
  uint32_t *p2 = (uint32_t *)((vaddr_t)test_buf | 0xa0000000);
  uint32_t *p;
  int i, j;

  iprintf ("Dump from P2\n");
  for (j = 0, p = p2; j < 1; j++)
    {
      for (i = 0; i < 8; i++)
	iprintf ("%x ", *p++);
      iprintf ("\n");
    }
}

uint32_t
cache (int32_t argc, const char *argv[])
{
  struct {
    void (*cache_func)(vaddr_t, vsize_t);
    uint8_t pattern;
    const char *name;
  } cache_test [] = {
    { sh4a_dcache_wbinv_range, 0xaa, "write-back invalidate" },
    { sh4a_dcache_wb_range, 0xbb, "write-back" },
    { sh4a_dcache_inv_range, 0xcc, "invalidate" },
    { sh4a_dcache_inv_range_index, 0xc0, "invalidate(index)" },
    { sh4a_dcache_wbinv_range_index, 0xdd, "write-back invalidate index" },
    { (void(*)(vaddr_t, vsize_t))sh4a_dcache_wbinv_all, 0xee,
      "write-back invalidate index all" },
  }, *p;
  //#define	CACHE_SIMPLE_TEST

#ifndef CACHE_SIMPLE_TEST
  vsize_t test_buf_size = 32;
#endif
  if (argc < 2)
    return 0;
  p = cache_test + atoi (argv[1]);
  cpu_status_t s = intr_suspend ();
#ifdef CACHE_SIMPLE_TEST
  p->cache_func (0, 8);
#else
  memset (test_buf + 0x2000, 0xa5, test_buf_size);

  iprintf ("%s test buffer P1=%x pattern=%x\n", p->name, test_buf, p->pattern);
  sh4a_dcache_array_dump ((vaddr_t)test_buf, test_buf_size);
  memset (test_buf, p->pattern, test_buf_size);
  sh4a_dcache_array_dump ((vaddr_t)test_buf, test_buf_size);
  dump_from_p2 ();

  p->cache_func ((vaddr_t)test_buf, test_buf_size);

  dump_from_p2 ();
  sh4a_dcache_array_dump ((vaddr_t)test_buf, test_buf_size);
#endif
  intr_resume (s);

  return 0;
}

void
deffered_func (void *ctx)
{
  thread_t th = (thread_t)ctx;

  thread_wakeup (th);
  iprintf ("wakeup!\n");
}

uint32_t
timer (int32_t argc __attribute__((unused)),
     const char *argv[] __attribute__((unused)))
{

  timer_schedule_func (deffered_func, (void *)test_th, 1000*1000);

  return 0;
}
