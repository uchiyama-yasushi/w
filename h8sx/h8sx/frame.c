
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
#include <sys/console.h>

#ifdef DEBUG
void
frame_dump (uint8_t *frame)
{
  struct switch_frame *sf;
  struct exception_frame *ef;
  struct exception_stack_frame *esf;
  bool is_exception_frame = frame[0] & 0x1;

  iprintf ("frame flags: %x thread_state=%x\n", frame[0], frame[1]);
  if (is_exception_frame)
    {
      ef = (struct exception_frame *)frame;
      esf = &ef->stack_frame;
      iprintf ("Exception frame\ner0:%x\ner1:%x\ner2:%x\ner3:%x\n"
	       "er4:%x\ner5:%x\ner6:%x\n\n",
	       ef->er0, ef->er1, ef->er2, ef->er3,
	       ef->er4, ef->er5, ef->er6);
    }
  else
    {
      sf = (struct switch_frame *)frame;
      esf = &sf->stack_frame;
      iprintf ("Switch frame\ner4:%x\ner5:%x\ner6:%x\n\n",
	       sf->er4, sf->er5, sf->er6);
    }
  uint32_t ccr_pc = esf->ccr_pc;

  iprintf ("exr=%x\nccr=%x\npc=%x\n", esf->exr, ccr_pc >> 24, ccr_pc & 0xffffff);
}

#ifdef EXT_FRAME_INFO
void
frame_exception_assert (struct exception_frame *ef, uint8_t opri, uint8_t pri)
{
  uint8_t epri = ef->stack_frame.exr & 0x7;

  if (epri != opri)
    {
      iprintf ("XXX Invalid exception frame %d != %d\n", epri, opri);
      frame_dump ((uint8_t *)ef);
    }

  if (opri & 0x7)
    iprintf ("[NESTED]");

  iprintf ("opri=%d -> pri=%d :", opri, pri);
}
#endif
#endif
