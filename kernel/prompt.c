
/*-
 * Copyright (c) 2009, 2011 UCHIYAMA Yasushi.  All rights reserved.
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

// Emacs like command line edit.
#include <sys/system.h>
#include <string.h>
#include <stdio.h>
#include <sys/prompt.h>

STATIC struct prompt_buffer
{
  int8_t buf[PROMPT_BUFFER_SIZE];
  int8_t cursor, min, max;
} prompt_buffer[PROMPT_HISTORY_MAX];

STATIC struct prompt_buffer *prompt;
STATIC int prompt_buffer_index;
STATIC struct prompt_buffer *__prompt_reset ();
STATIC struct _file *__prompt_out;

void
prompt_init (struct _file *f)
{

  __prompt_out = f;

  prompt = __prompt_reset ();
}

void
prompt_reset ()
{

  prompt = __prompt_reset ();
}

int8_t *
prompt_input (int8_t c)
{
#ifndef PROMPT_NOEDIT
  int i;
#endif

  switch (c)
    {
#ifdef PROMPT_NOEDIT
    default:
      prompt->buf[prompt->cursor] = c;
      prompt->cursor =
	(prompt->cursor >= PROMPT_BUFFER_SIZE - 1) ? prompt->cursor :
	prompt->cursor + 1;
      if (prompt->cursor >= prompt->max)
	prompt->max = prompt->cursor;
      fputc (c, __prompt_out);
      break;
    case '\r':
      fputc ('\n', __prompt_out);
      if (prompt->max > 0)
	return prompt->buf;	// Execute command.
      else
	fprintf (__prompt_out, PROMPT);
      break;
#else //PROMPT_NOEDIT
    case 32 ... 127:		// Normal strings.
      prompt->buf[prompt->cursor] = c;
      prompt->cursor =
	(prompt->cursor >= PROMPT_BUFFER_SIZE - 1) ? prompt->cursor :
	prompt->cursor + 1;
      if (prompt->cursor >= prompt->max)
	prompt->max = prompt->cursor;
      fputc (c, __prompt_out);
      break;

    case '\r':
      fputc ('\n', __prompt_out);
      if (prompt->max > 0)
	return prompt->buf;	// Execute command.
      else
	fprintf (__prompt_out, PROMPT);
      break;
    case '\b':			// Back space.
      if (prompt->cursor > 0)
	{
	  --prompt->cursor;
	  for (i = prompt->cursor; i < prompt->max; i++)
	    prompt->buf[i] = prompt->buf[i + 1];
	}
      goto redraw;
      //
      // Emacs key-bind.
      //
    case 1:		/* Ctrl a */
      prompt->cursor = prompt->min;
      fputc ('\r', __prompt_out);
      fprintf (__prompt_out, "%s", PROMPT);
      break;

    case 5:		/* Ctrl e */
      prompt->cursor = prompt->max;
      goto redraw;

    case 2:		/* Ctrl b */
      if (prompt->cursor == prompt->min)
	return 0;
      prompt->cursor--;
      fputc ('\b', __prompt_out);
      break;

    case 6:		/* Ctrl f */
      if (prompt->cursor == prompt->max)
	return 0;
      prompt->cursor++;
      fprintf (__prompt_out, "\r%s", PROMPT);
      for (i = 0; i < prompt->cursor; i++)
	fputc (prompt->buf[i], __prompt_out);
      break;

    case 4:		/* Ctrl d */
      if (prompt->cursor == prompt->max)
	return 0;
      for (i = prompt->cursor; i < prompt->max; i++)
	prompt->buf[i] = prompt->buf[i + 1];
      prompt->buf[i] = '\0';
      prompt->max--;
      goto redraw;

    case 11:		/* Ctrl k */
      for (i = prompt->cursor; i < prompt->max; i++)
	{
	prompt->buf[i] = 0;
	prompt->max = prompt->cursor;
	}
      goto redraw;

    case 14:		/* Ctrl n */
      prompt_buffer_index = (prompt_buffer_index + 1) & PROMPT_HISTORY_MASK;
      goto history_redraw;

    case 16:		/* Ctrl p */
      prompt_buffer_index = (prompt_buffer_index - 1) & PROMPT_HISTORY_MASK;
    history_redraw:
      prompt = &prompt_buffer[prompt_buffer_index];
      prompt->cursor = prompt->max;

    case 12:		/* Ctrl l */
    redraw:
      fputc ('\r', __prompt_out);
      for (i = 0; i < PROMPT_BUFFER_SIZE; i++)
	fputc (' ', __prompt_out);
      fputc ('\r', __prompt_out);
      fprintf (__prompt_out, "%s%s", PROMPT, prompt->buf);
      fprintf (__prompt_out, "\r%s", PROMPT);
      for (i = 0; i < prompt->cursor; i++)
	fputc (prompt->buf[i], __prompt_out);
#endif //PROMPT_NOEDIT
    }

  return 0;
}

struct prompt_buffer *
__prompt_reset ()
{
  struct prompt_buffer *b;

  prompt_buffer_index = (prompt_buffer_index + 1) & PROMPT_HISTORY_MASK;
  b = &prompt_buffer[prompt_buffer_index];
  memset (b, 0, sizeof *b);
  fprintf (__prompt_out, PROMPT);

  return b;
}
