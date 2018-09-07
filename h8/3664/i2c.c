
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

// XXX 3664 I2C is too buggy module.

#include <sys/system.h>
#include <reg.h>
#ifdef DEBUG
#include <sys/console.h>
#endif

void
i2c_init ()
{
  //I2C Disable. I2C_SAR and I2C_SARX are accessible.
  *I2C_ICCR = 0;
  // I2C format. only SAR used as slave address.
  *I2C_SAR = 0;	// Slave addr
  *I2C_SARX = SAR_FSX;	// Don't use SARX

  //I2C Enable.
  *I2C_ICCR |= ICCR_ICE;	// read required

  // Now register ICMR and ICDR are accessible.
  *I2C_ICMR = ICMR_CKS0;// CCLK16MHz 400kHz 8bit MSB1st
  //  *I2C_ICMR = ICMR_CKS0 | ICMR_CKS1 | ICMR_CKS2;// CCLK16MHz 125kHz 8bit MSB1st
  *I2C_TSCR = 0; //IICX = 0, IICRST = 0(noreset)
}

void
i2c_start ()
{
  uint8_t r;

  // Check bus free.
  while (*I2C_ICCR & ICCR_BBSY)
    ;
  // Master transfer mode.
  *I2C_ICCR |= ICCR_MST | ICCR_TRS;

  // Start condition.
  r = *I2C_ICCR;
  r |= ICCR_BBSY;
  r &= ~ICCR_SCP;
  *I2C_ICCR = r;

  // Wait until start condition is generated.
  while (!(*I2C_ICCR & ICCR_IRIC))
    ;
}

void
i2c_repeated_start ()
{
  uint8_t r;

  // Restart condition.
  r = *I2C_ICCR;
  r |= ICCR_BBSY;
  r &= ~ICCR_SCP;
  // Don't reorder here.
  r &= ~ICCR_IRIC;
  *I2C_ICCR = r;
  // Wait until start condition is generated.
  while (!(*I2C_ICCR & ICCR_IRIC))
    ;
}

void
i2c_stop ()
{

  *I2C_ICCR &= ~ICCR_IRIC;
  *I2C_ICCR &= ~(ICCR_BBSY | ICCR_SCP);
}

bool
i2c_write (uint8_t data)
{
  // Transmit data.
  *I2C_ICDR = data;
  *I2C_ICCR &= ~ICCR_IRIC;
  while (!(*I2C_ICCR & ICCR_IRIC))
    ;

  // Transmit done.
  // Check Ack from slave.
  if (*I2C_ICSR & ICSR_ACKB)
    {
#ifdef DEBUG
      iprintf ("no ack\n");
#endif
      return FALSE;
    }
  // Ack received.

  return TRUE;
}

void
i2c_read_n (uint8_t *buf, int n)
{
  int i;

  // Master receive mode.
  *I2C_ICCR &= ~ICCR_TRS;
  *I2C_ICMR |= ICMR_WAIT;
  *I2C_ICSR &= ~ICSR_ACKB;

  //XXX multiple read may fail and stall I2C module.
  for (i = 0; i < n; i++)
    {
      *I2C_ICCR &= ~ICCR_IRIC;
      buf[i] = *I2C_ICDR;
      while (!(*I2C_ICCR & ICCR_IRIC))
	;
    }

  // NACK
  *I2C_ICSR |= ICSR_ACKB;
  *I2C_ICCR |= ICCR_TRS;
  *I2C_ICCR &= ~ICCR_IRIC;
  while (!(*I2C_ICCR & ICCR_IRIC))
    ;
  *I2C_ICMR &= ~ICMR_WAIT;
  //  r = *I2C_ICDR;
  *I2C_ICCR &= ~ICCR_IRIC;
}

void
i2c_status (const char *s __attribute__((unused)))
{
#ifdef DEBUG
  uint8_t iccr, icsr;
  iccr = *I2C_ICCR;
  icsr = *I2C_ICSR;

  iprintf ("CR:%x SR:%x %s\n", iccr, icsr, s);
#endif
}
