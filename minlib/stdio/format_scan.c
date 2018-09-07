
/*-
 * Copyright (c) 2011 UCHIYAMA Yasushi.  All rights reserved.
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

/* Taken from 2.10BSD */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)doscan.c	5.2 (Berkeley) 3/9/86";
#endif //LIBC_SCCS and not lint
#include <sys/system.h>
#include <sys/stream.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#define	SPC	01
#define	STP	02

#define	SHORT	0
#define	REGULAR	1
#define	LONG	2
#define	INT	0
#define	FLOAT	1

static char _sctab[256] =
  {
    0,0,0,0,0,0,0,0,
    0,SPC,SPC,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    SPC,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
  };

STATIC char *_getccl();
STATIC int _innum (struct _stream_char_ops *, int *, int, int, int, int *);
STATIC int _instr (struct _stream_char_ops *, char *, int, int, int *);

int
format_scan (struct _stream_char_ops *iop, const char *fmt, va_list argp)
{
  int ch;
  int nmatch, len, ch1;
  int *ptr, fileended, size;

  nmatch = 0;
  fileended = 0;
  while (/*CONSTCOND*/1)
    {
      switch (ch = *fmt++)
	{
	case '\0':
	  return nmatch;
	case '%':
	  if ((ch = *fmt++) == '%')
	    goto def;
	  ptr = 0;
	  if (ch != '*')
	    ptr = va_arg (argp, int *);
	  else
	    ch = *fmt++;
	  len = 0;
	  size = REGULAR;

	  while (isdigit (ch))
	    {
	      len = len * 10 + ch - '0';
	      ch = *fmt++;
	    }
	  if (len == 0)
	    len = 30000;

	  if (ch=='l')
	    {
	      size = LONG;
	      ch = *fmt++;
	    }
	  else if (ch=='h')
	    {
	      size = SHORT;
	      ch = *fmt++;
	    }
	  else if (ch=='[')
	    {
	      fmt = _getccl(fmt);
	    }

	  if (isupper(ch))
	    {
	      ch = tolower(ch);
	      size = LONG;
	    }
	  if (ch == '\0')
	    return -1;

	  if (_innum (iop, ptr, ch, len, size, &fileended) && ptr)
	    nmatch++;

	  if (fileended)
	    return nmatch ? nmatch : -1;

	  break;

	case ' ':
	case '\n':
	case '\t':
	  while ((ch1 = (*iop->getc) (iop->ctx)) ==' ' || ch1=='\t' || ch1=='\n')
	    ;
	if (ch1 != EOF)
	  (*iop->ungetc) (iop->ctx, ch1);
	break;

	default:
	def:
	  ch1 = (*iop->getc) (iop->ctx);
	  if (ch1 != ch)
	    {
	      if (ch1 == EOF)
		return -1;
	      (*iop->ungetc) (iop->ctx, ch1);
	      return nmatch;
	    }
	}
    }
}

int
_innum (struct _stream_char_ops *iop, int *ptr, int type, int len, int size,
	int *eofptr)
{
  extern double atof();
  char *np;
  char numbuf[64];
  int c, base;
  int expseen, scale, negflg, c1, ndigit;
  long lcval;

  if (type=='c' || type=='s' || type=='[')
    return _instr (iop, ptr ? (char *)ptr : 0, type, len, eofptr);

  lcval = 0;
  ndigit = 0;
  scale = INT;
  if (type == 'e' || type == 'f')
    scale = FLOAT;

  base = 10;
  if (type=='o')
    {
      base = 8;
    }
  else if (type=='x')
    {
      base = 16;
      if ((c = (*iop->getc) (iop->ctx)) == '0')
	(*iop->getc) (iop->ctx);	// skip '0x'
      else
	(*iop->ungetc) (iop->ctx, c);
    }

  np = numbuf;
  expseen = 0;
  negflg = 0;
  while ((c = (*iop->getc) (iop->ctx)) == ' ' || c == '\t' || c == '\n')
    ;

  if (c == '-')
    {
      negflg++;
      *np++ = c;
      c = (*iop->getc) (iop->ctx);
      len--;
    }
  else if (c=='+')
    {
      len--;
      c = (*iop->getc) (iop->ctx);
    }

  for (; --len >= 0; *np++ = c, c = (*iop->getc) (iop->ctx))
    {
      if (isdigit (c) ||
	  (base == 16 && (('a' <= c && c <= 'f') ||( 'A' <= c && c <= 'F'))))
	{
	  ndigit++;
	  if (base == 8)
	    lcval <<= 3;
	  else if (base == 10)
	    lcval = ((lcval << 2) + lcval) << 1;
	  else
	    lcval <<= 4;
	  c1 = c;
	  if (isdigit (c))
	    c -= '0';
	  else if ('a'<= c && c <= 'f')
	    c -= 'a' - 10;
	  else
	    c -= 'A' - 10;
	  lcval += c;
	  c = c1;
	  continue;
	}
      else if (c == '.')
	{
	  if (base != 10 || scale == INT)
	    break;
	  ndigit++;
	  continue;
	}
      else if ((c=='e'||c=='E') && expseen==0) 
	{
	  if (base != 10 || scale == INT || ndigit == 0)
	    break;
	  expseen++;
	  *np++ = c;
	  c = (*iop->getc) (iop->ctx);
	  if (c != '+' && c != '-' && ('0' > c || c > '9'))
	    break;
	}
      else
	break;
    }
  if (negflg)
    lcval = -lcval;
  if (c != EOF)
    {
      (*iop->ungetc) (iop->ctx, c);
      *eofptr = 0;
    }
  else
    *eofptr = 1;
  if (ptr == 0 || np == numbuf || (negflg && np == numbuf + 1) )/* gene dykes*/
    return 0;
  *np++ = 0;
  switch ((scale << 4) | size)
    {

    case (FLOAT<<4) | SHORT:
    case (FLOAT<<4) | REGULAR:
      *(float *)ptr = atof (numbuf);
      break;

    case (FLOAT<<4) | LONG:
      *(double *)ptr = atof (numbuf);
      break;

    case (INT<<4) | SHORT:
      *(short *)ptr = lcval;
      break;

    case (INT<<4) | REGULAR:
      *(int *)ptr = lcval;
      break;

    case (INT<<4) | LONG:
      *(long *)ptr = lcval;
      break;
    }

  return 1;
}

int
_instr (struct _stream_char_ops *iop, char *ptr, int type, int len, int *eofptr)
{
  int ch;
  char *optr;
  int ignstp;

  *eofptr = 0;
  optr = ptr;
  if (type == 'c' && len == 30000)
    len = 1;
  ignstp = 0;
  if (type == 's')
    ignstp = SPC;

  while ((ch = (*iop->getc) (iop->ctx)) != EOF && _sctab[ch] & ignstp)
    ;
  ignstp = SPC;

  if (type == 'c')
    ignstp = 0;
  else if (type == '[')
    ignstp = STP;

  while (ch!=EOF && (_sctab[ch]&ignstp) == 0)
    {
      if (ptr)
	*ptr++ = ch;
      if (--len <= 0)
	break;
      ch = (*iop->getc) (iop->ctx);
    }

  if (ch != EOF)
    {
      if (len > 0)
	(*iop->ungetc) (iop->ctx, ch);
      *eofptr = 0;
    }
  else
    {
      *eofptr = 1;
    }

  if (ptr && ptr != optr)
    {
      if (type != 'c')
	*ptr++ = '\0';
      return 1;
    }

  return 0;
}

char *
_getccl(unsigned char *s)
{
  int t;
  size_t c;

  t = 0;
  if (*s == '^')
    {
      t++;
      s++;
    }

  for (c = 0; c < (sizeof _sctab / sizeof _sctab[0]); c++)
    if (t)
      _sctab[c] &= ~STP;
    else
      _sctab[c] |= STP;

  if ((c = *s) == ']' || c == '-') /* first char is special */
    {
      if (t)
	_sctab[c] |= STP;
      else
	_sctab[c] &= ~STP;
      s++;
    }

  while ((c = *s++) != ']')
    {
      if (c == 0)
	{
	  return((char *)--s);
	}
      else if (c == '-' && *s != ']' && s[-2] < *s)
	{
	  for (c = s[-2] + 1; c < *s; c++)
	    if (t)
	      _sctab[c] |= STP;
	    else
	      _sctab[c] &= ~STP;
	}
      else if (t)
	{
	  _sctab[c] |= STP;
	}
      else
	{
	  _sctab[c] &= ~STP;
	}
    }

  return (char *)s;
}
