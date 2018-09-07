
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


#include <sys/system.h>
#include <sys/types.h>
#include <sys/stream.h>
#include <stdarg.h>
#include <ctype.h>

struct print_format
{
  uint8_t flags;
#define	PRT_ALTERNATE_FORM	0x01
#define	PRT_LEFT_ADJUST		0x02
#define	PRT_ZERO_PADDING	0x04
  char sign_char;
  int8_t width;
  int8_t precision;
};

STATIC const char *format_option_digits (const char *, int8_t *);
STATIC const char *parse_format_option (const char *, va_list *,
					struct print_format *);
STATIC const char *parse_format_and_print (struct _stream_char_ops *,
					   const char *, va_list *,
					   struct print_format *);
STATIC void print_string (struct _stream_char_ops *, char *,
			  struct print_format *);
STATIC void print_number (struct _stream_char_ops *, int32_t,
			  struct print_format *, int, bool);
STATIC char *prepare_digit_buffer (char *, size_t, uint32_t, int);
#ifdef DO_PRINT_DEBUG
STATIC void dump_format_option (struct print_format *);
#endif

void
format_print (struct _stream_char_ops *iop, const char *fmt, va_list ap)
{
  struct print_format pf;
  char c;
  void (*_putc)(void *, int8_t) = iop->putc;
  void *ctx = iop->ctx;

  while (/*CONSTCOND*/1)
    {
      while ((c = *fmt++) != '%')
	{
	  if (c == '\0')
	    return;
	  _putc (ctx, c);
	}
      // Set default.
      pf.flags = 0;
      pf.precision = 0x7f;
      pf.width = 0;
      pf.sign_char = 0;
#ifdef __amd64__
      fmt = parse_format_option (fmt, (va_list *)ap, &pf);
      fmt = parse_format_and_print (iop, fmt, (va_list *)ap, &pf);
#else
      fmt = parse_format_option (fmt, &ap, &pf);
      fmt = parse_format_and_print (iop, fmt, &ap, &pf);
#endif
    }
}

const char *
parse_format_option (const char *fmt, va_list *ap, struct print_format *pf)
{
  /*
     alternate form (#)
     sign character (+)
     blank character before signed number ( )
  */
  char c;
  int8_t flags;

  for (flags = 0; (c = *fmt); fmt++)
    {
      switch (c)
	{
	default:
	  goto phase2;
	case '#':
	  flags |= PRT_ALTERNATE_FORM;
	  continue;
	case '-':
	  flags |= PRT_LEFT_ADJUST;
	  continue;
	case ' ':
	  if (!pf->sign_char)	// weaker than '+'
	    pf->sign_char = ' ';
	  continue;
	case '+':
	  pf->sign_char = '+';
	  continue;
	case '0':
	  flags |= PRT_ZERO_PADDING;
	  continue;
	}
    }
 phase2:

  while ((c = *fmt))
    {
      switch (c)
	{
	default:
	  goto phase3;
	  // Field width
	case '1' ... '9':
	  fmt = format_option_digits (fmt, &pf->width);
	  continue;
	case '*':
	  pf->width = va_arg (*ap, int);
	  if (pf->width < 0) // left adjust
	    {
	      pf->width = -pf->width;
	      flags ^= PRT_LEFT_ADJUST;// xor for '%-*'
	    }
	  fmt++;
	  continue;
	  // Precision
	case '.':
	  fmt++;
	  if (isdigit (*fmt))
	    {
	      fmt = format_option_digits (fmt, &pf->precision);
	    }
	  else if (*fmt == '*')
	    {
	      pf->precision = va_arg (*ap, int);
	      fmt++;
	    }
	}
    }
 phase3:

  pf->flags = flags;

  return fmt;
}

const char *
parse_format_and_print (struct _stream_char_ops *iop, const char *fmt,
			va_list *ap, struct print_format *pf)
{
  void (*_putc)(void *, int8_t) = iop->putc;
  void *ctx = iop->ctx;
  int32_t n;
  char i;
  bool force32 = FALSE;

 again:
  switch (i = *fmt++)
    {
    default:
      _putc (ctx, i);
      break;
    case 'u':
      goto again;
    case 'l':
      force32 = TRUE;	// for architecture that sizeof (int) is 2byte.
      goto again;
    case 'c':
      i = va_arg (*ap, int);
      _putc (ctx, i & 0x7f);
      break;
    case 's':
      print_string (iop, va_arg (*ap, char *), pf);
      break;
    case 'o':
      print_number (iop, va_arg (*ap, int), pf, 8, FALSE);
      break;
    case 'd':
      n = force32 ? va_arg (*ap, int32_t) : va_arg (*ap, int);
      if (n < 0)
	{
	  n = -n;
	  pf->sign_char = '-';
	}
      print_number (iop, n, pf, 10, TRUE);
      break;
    case 'p':
#if defined __H8300H__ && defined __NORMAL_MODE__
      n = va_arg (*ap, unsigned int);//XXX fuck! addr_t is unsigned int
#else
      n = va_arg (*ap, addr_t);
#endif
      pf->flags |= PRT_ALTERNATE_FORM;
      print_number (iop, n, pf, 16, FALSE);
      break;
    case 'x':
      n = force32 ? va_arg (*ap, int32_t) : va_arg (*ap, int);
      print_number (iop, n, pf, 16, FALSE);
      break;
    case '\0':
      fmt--;
      break;
    }

  return fmt;
}

void
print_string (struct _stream_char_ops *iop, char *s, struct print_format *pf)
{
  int i, precision, width;
  bool left_adjust;
  char c;
  void (*_putc)(void *, int8_t) = iop->putc;
  void *ctx = iop->ctx;

  assert (s);

  precision = pf->precision;
  width = pf->width;
  left_adjust = pf->flags & PRT_LEFT_ADJUST;

  // Right padding
  if (!left_adjust && width > 0) //width specified.
    {
      // calculate string length.
      for (i = 0; s[i] && (i < precision); i++)
	;
      // padding
      for (; i < width; i++)
	_putc (ctx, ' ');
    }

  // Print body
  for (i = 0; (c = s[i]) && i < precision; i++)
    _putc (ctx, c);

  // Left padding
  if (left_adjust)
    for (; i < width; i++)
      _putc (ctx, ' ');
}

void
print_number (struct _stream_char_ops *iop, int32_t n, struct print_format *pf,
	      int base, bool sign)
{
  char buf[16];
  bool left_adjust = pf->flags & PRT_LEFT_ADJUST;
  bool alternate_form = pf->flags & PRT_ALTERNATE_FORM;
  int width = pf->width;
  char sign_char = pf->sign_char;
  char *p;
  char c;
  int len;

  // Sign adjust and setup sign character ' ', '-', '+'
  if (sign_char)
    {
      width--;
    }
  else if (alternate_form)
    {
      width -= (base >> 3); // Decrement '0x' or '0'
    }

  // Digit itself
  p = prepare_digit_buffer (buf, sizeof buf, (uint32_t)n, base);
  len = buf + sizeof buf - p - 2;

  // Zero padding
  if (pf->flags & PRT_ZERO_PADDING)
    {
      for (width -= len; width > 0; width--)
	*p-- = '0';
    }

  // Add prefix / sign character
  if (sign && sign_char)
    {
      *p-- = sign_char;
    }
  else if (alternate_form)
    {
      if (base == 16)
	*p-- = 'x';
      *p-- = '0';
    }

  // Now write out to file.
  void (*_putc)(void *, int8_t) = iop->putc;
  void *ctx = iop->ctx;
  width -= len;
  // Left padding
  if (!left_adjust)
    while (width-- > 0)
      _putc (ctx, ' ');

  // Body
  while ((c = *++p))
    _putc (ctx, c);

  // Right padding
  if (left_adjust)
    while (width-- > 0)
      _putc (ctx, ' ');
}

char *
prepare_digit_buffer (char *buf, size_t sz, uint32_t u, int base)
{
  const char hexdigits[] = "0123456789abcdef";
  char *p = buf + sz - 1;

  *p-- = '\0';	// terminater
  do
    {
      *p-- = hexdigits[u % base];
    }
  while (u /= base);

  return p;
}

const char *
format_option_digits (const char *fmt, int8_t *n)
{
  int i = 0;
  char c;

  for (i = 0; isdigit ((c = *fmt)); fmt++)
    i = i * 10 + c - '0';

  *n = i;

  return fmt;
}
#ifdef DO_PRINT_DEBUG
void
dump_format_option (struct print_format *pf)
{
  uint8_t flags = pf->flags;
  printf ("%s|%s|%s [%c] width=%d precision=%d\n",
	  flags & PRT_ALTERNATE_FORM ? "altfmt" : "",
	  flags & PRT_LEFT_ADJUST ? "ladjust" : "",
	  flags & PRT_ZERO_PADDING ? "zeropad" : "",
	  pf->sign_char, pf->width, pf->precision);
}
#endif // DO_PRINT_DEBUG

#ifdef SELFTEST
void
my_putc (void *ctx, int8_t c)
{
  int putchar (int);
  putchar (c);
}

void
my_printf (const char *fmt, ...)
{
  va_list ap;

  struct _stream_char_ops ops;
  ops.putc = my_putc;
  va_start (ap, fmt);
  format_print (&ops, fmt, ap);
  va_end (ap);
}

int
main ()
{
  my_printf ("%10s\n", "ohayo");
  my_printf ("%-10s\n", "ohayo");
  my_printf ("%-10.3s\n", "ohayo");
  my_printf ("%10.4s\n", "ohayo");
  printf ("------------------------------\n");
  printf ("%10s\n", "ohayo");
  printf ("%-10s\n", "ohayo");
  printf ("%-10.3s\n", "ohayo");
  printf ("%10.4s\n", "ohayo");


  my_printf ("hello world\n");
  my_printf ("%x %x %d %d %d\n", 0xac1dcafe, 0x1badface, 14, 28, 99);
  printf ("%x %x %d %d %d\n", 0xac1dcafe, 0x1badface, 14, 28, 99);
  my_printf ("%x\n", 0xdeadbeaf);
  my_printf ("%d\n", 123456);
  my_printf ("%c\n", 'n');
  my_printf ("%s\n", "ohayo");

  my_printf ("%08x\n", 0x34);
  my_printf ("%8x\n", 0x34);
  my_printf ("%#08x\n", 0x34);
  my_printf ("%#8x\n", 0x34);
  my_printf ("%#-8x\n", 0x34);
  my_printf ("%#010x\n", 0x34);
  my_printf ("%x\n", 0x34);
  my_printf ("%d\n", 45);
  my_printf ("%d\n", -45);
  my_printf ("%8d\n", -45);
  my_printf ("%-8d\n", -45);

  printf ("---\n");
  printf ("%08x\n", 0x34);
  printf ("%8x\n", 0x34);
  printf ("%#08x\n", 0x34);
  printf ("%#-8x\n", 0x34);
  printf ("%#010x\n", 0x34);
  printf ("%x\n", 0x34);
  printf ("%d\n", 45);
  printf ("%d\n", -45);
  printf ("%8d\n", -45);
  printf ("%-8d\n", -45);


  return 0;
}
#endif	//SELFTEST
