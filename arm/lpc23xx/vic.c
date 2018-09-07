
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

// VIC Vectored Interrupt Controller.

#include <sys/system.h>
#include <sys/console.h>
#include <reg.h>

typedef void (irq_handler_t)(void);
// User program can override these place holder.
__BEGIN_DECLS
void user_irq_handler (void);
void user_fiq_handler (void);

irq_handler_t null_intr;
irq_handler_t wdt_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t soft_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t dbgcommrx_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t dbgcommtx_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t timer0_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t timer1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t uart0_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t uart1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t pwm1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t i2c0_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t spi_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t ssp1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t pll_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t rtc_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t eint0_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t eint1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t eint2_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t eint3_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t adc0_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t i2c1_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t bod_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t ether_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t usb_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t can_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t sdmmc_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t gpdma_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t timer2_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t timer3_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t uart2_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t uart3_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t i2c2_intr __attribute__ ((weak, alias ("null_intr")));
irq_handler_t i2s_intr __attribute__ ((weak, alias ("null_intr")));
__END_DECLS

irq_handler_t *irq_vector[] =
  {
    wdt_intr,		//0
    soft_intr,		//1
    dbgcommrx_intr,	//2
    dbgcommtx_intr,	//3
    timer0_intr,	//4
    timer1_intr,	//5
    uart0_intr,		//6
    uart1_intr,		//7
    pwm1_intr,		//8
    i2c0_intr,		//9
    spi_intr,		//10
    ssp1_intr,		//11
    pll_intr,		//12
    rtc_intr,		//13
    eint0_intr,		//14
    eint1_intr,		//15
    eint2_intr,		//16
    eint3_intr,		//17
    adc0_intr,		//18
    i2c1_intr,		//19
    bod_intr,		//20
    ether_intr,		//21
    usb_intr,		//22
    can_intr,		//23
    sdmmc_intr,		//24
    gpdma_intr,		//25
    timer2_intr,	//26
    timer3_intr,	//27
    uart2_intr,		//28
    uart3_intr,		//29
    i2c2_intr,		//30
    i2s_intr,		//31
  };

void
vic_init ()
{
  volatile uint32_t *vic_addr = VICVectAddr;
  volatile uint32_t *vic_priority = VICVectPriority;
  int i;

  cpu_status_t s = intr_suspend ();

  // Interupt disabled.
  *VICIntEnClr = 0xffffffff;
  // Clear software interrupt.
  *VICSoftIntClear = 0xffffffff;

  // Install interrupt handler.
  for (i = 0; i < 32; i++)
    {
      vic_addr[i] = (uint32_t)irq_vector [i];
      vic_priority[i] = 0xf;	// Lowest;
    }

  *VICSWPriorityMask = 0xffff;	// All interrupt level are not masked.
  *VICProtection = 0;	// No protection.
  intr_resume (s);
}

void
vic_protect (bool protect)
{
  cpu_status_t s = intr_suspend ();
  *VICProtection = protect ? 1 : 0;
  intr_resume (s);
}

int
vic_priority_set (int irq, int priority)
{
  cpu_status_t s = intr_suspend ();
  int opri = VICVectPriority[irq];

  VICVectPriority[irq] = priority;
  intr_resume (s);
  return opri;
}

int
vic_priority_mask (int priority)
{
  cpu_status_t s = intr_suspend ();
  int omask = *VICSWPriorityMask;

  *VICSWPriorityMask = priority;
  intr_resume (s);

  return omask;
}

void
vic_interrupt_enable (int src, enum vic_irq_type type)
{
  cpu_status_t s = intr_suspend ();
  if (type == VIC_IRQ)
    *VICIntSelect &= ~src; // IRQ
  else
    *VICIntSelect |= src; // IRQ

  *VICIntEnable |= src;
  intr_resume (s);
}

void
vic_interrupt_disable (int src)
{
  cpu_status_t s = intr_suspend ();
  *VICIntEnable &= ~src;
  intr_resume (s);
}

void
user_fiq_handler ()
{
  uint32_t r = *VICFIQStatus;
  uint32_t i;

  for (i = 0; i < 32; i++)
    if (r & (1 << i))
      (*irq_vector[i]) ();
}

void
user_irq_handler ()
{
  irq_handler_t *handler = (irq_handler_t *)*VICAddress;
  //  iprintf ("%x\n", handler);
  handler ();

  *VICAddress = 0; // Acknowledge.
}

void
null_intr ()
{

  iprintf ("%s: %x\n", __FUNCTION__,  *VICIRQStatus);
  while (/*CONSTCOND*/1)
    ;
  // NOTREACHED
}



