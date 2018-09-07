
/*-
 * Copyright (c) 2009-2010 UCHIYAMA Yasushi.  All rights reserved.
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

/* Motorola S-format loader */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "h8sxwrite.h"

enum srec_error
  {
    SREC_OK,
    SREC_EOF,
    SREC_MISSING_START,
    SREC_INVALID_CHAR,
    SREC_CHECKSUM_ERROR,
    SREC_READ_ERROR,
    SREC_NOTSUPPORTED_TYPE,
    SREC_BUFFER_OVERFLOW,
  };
typedef enum srec_error srec_return_t;

const char *srec_error_message[] =
{
  "*file shotage*",
  "success",
  "*missing start*",
  "*invalid character*",
  "*checksum error*",
  "*read error*",
  "*not supported type*",
  "*buffer over flow*",
};
addr_t srec_start_addr;

STATIC int __srec_type;
STATIC int __srec_count_max;
STATIC uint8_t __srec_buffer[2];
STATIC int __srec_buffer_index;
STATIC int __srec_count;
STATIC int __srec_address_count;
STATIC uint32_t __srec_checksum;
STATIC addr_t __srec_load_addr;	// Advanced-mode: 32bit, Normal-mode:16bit
STATIC uint8_t __srec_address_length [10] = { 2, 2, 3, 4, -1, 2, -1, 4, 3, 2 };
STATIC srec_return_t (*srec_parse_func)(uint8_t);
STATIC srec_return_t srec_count (uint8_t);
STATIC srec_return_t srec_type (uint8_t);
STATIC srec_return_t srec_addr (uint8_t);
STATIC srec_return_t srec_data_ascii (uint8_t);
STATIC srec_return_t srec_data_load (uint8_t);
STATIC srec_return_t srec_checksum (uint8_t);
STATIC bool srec_read1 (uint8_t);
STATIC int32_t srec_getvalue (void);
STATIC int32_t srec_getvalue_nochecksum (void);
STATIC uint8_t atohex (uint8_t);

STATIC uint8_t *__srec_load_base;
STATIC size_t __srec_load_limit;

static inline bool __srec_is_start_addr (int type)
{

  return type >= 7 && type <= 9;
}

void
srec_init ()
{
  srec_start_addr = 0;
  __srec_buffer_index = 0;
}

srec_return_t
srec_parser (uint8_t c)
{

  switch (c)
    {
    case 'S':
      srec_parse_func = srec_type;
      break;
    case '\r':
    case '\n':
      srec_parse_func = NULL;
      break;
    case 'a'...'z':
    case 'G'...'R':
    case 'T'...'Z':
      return SREC_INVALID_CHAR;
    default:
      if (!srec_parse_func)
	{
	  return SREC_MISSING_START;
	}
      return srec_parse_func (c);
    }

  return SREC_OK;
}

srec_return_t
srec_type (uint8_t c)
{

  __srec_type = c - '0';
  DPRINTF ("type=%d ", __srec_type);
  // start checksum.
  __srec_checksum = 0;
  // next count phase.
  srec_parse_func = srec_count;

  // Supported record type.
  if ((__srec_type > 3) && (__srec_type < 7))
    return SREC_NOTSUPPORTED_TYPE;

  return SREC_OK;
}

srec_return_t
srec_count (uint8_t c)
{

  if (!srec_read1 (c))
    return SREC_OK;

  __srec_count_max = srec_getvalue ();
  __srec_count = 0;
  DPRINTF ("count=%d ", __srec_count_max);

  // next address phase.
  __srec_address_count = __srec_address_length[__srec_type];
  __srec_load_addr = 0;

  srec_parse_func = srec_addr;

  return SREC_OK;
}

srec_return_t
srec_addr (uint8_t c)
{
  uint32_t val;

  if (!srec_read1 (c))
    return SREC_OK;
  val = srec_getvalue ();

  __srec_load_addr |= val << (--__srec_address_count * 8);
  if (__srec_address_count == 0)
    {
      DPRINTF ("addr=[%x] ", __srec_load_addr);
      if (__srec_is_start_addr (__srec_type))
	{
	  srec_start_addr = __srec_load_addr;
	  srec_parse_func = srec_checksum;
	}
      else if (__srec_type == 0)
	{
	  srec_parse_func = srec_data_ascii;
	}
      else
	{
	  srec_parse_func = srec_data_load;
	}
    }
  return SREC_OK;
}

srec_return_t
srec_data_ascii (uint8_t c)
{
  int32_t val;

  if (!srec_read1 (c))
    return SREC_OK;
  val = srec_getvalue ();
  DPRINTF ("%c", val);

  return SREC_OK;
}

srec_return_t
srec_data_load (uint8_t c)
{
  int32_t val;

  if (!srec_read1 (c))
    return SREC_OK;
  val = srec_getvalue ();


  DPRINTF ("%02x|", val);

  if (__srec_load_addr < __srec_load_limit)
    {
      __srec_load_base[__srec_load_addr++] = val;
    }
  else
    {
      fprintf (stderr, "Invalid flash memory addr.(0x%x) SKIP.\n",
	       __srec_load_addr);
      //      return SREC_BUFFER_OVERFLOW;
    }

  return SREC_OK;
}

srec_return_t
srec_checksum (uint8_t c)
{
  uint32_t val;

  if (!srec_read1 (c))
    return SREC_OK;
  val = srec_getvalue_nochecksum ();

  srec_parse_func = NULL;	// end all phase.
  DPRINTF ("\n");

  if ((~(__srec_checksum & 0xff) & 0xff) != val)
    return SREC_CHECKSUM_ERROR;

  if (__srec_is_start_addr (__srec_type))
    return SREC_EOF;

  return SREC_OK;
}

bool
srec_read1 (uint8_t c)
{

  __srec_buffer[__srec_buffer_index] = c;
  __srec_buffer_index ^= 1;

  return !__srec_buffer_index;
}

int32_t
srec_getvalue ()
{
  int32_t val = atohex (__srec_buffer[0]) * 0x10 + atohex (__srec_buffer[1]);

  __srec_checksum += val;
  if (++__srec_count == __srec_count_max - 1)// end data phase.
    {
      srec_parse_func = srec_checksum;
    }

  return val & 0xff;
}

int32_t
srec_getvalue_nochecksum ()
{
  int32_t val = atohex (__srec_buffer[0]) * 0x10 + atohex (__srec_buffer[1]);

  return val & 0xff;
}

uint8_t
atohex (uint8_t c)
{

  return c > '@' ? c - '7' : c - '0';
}

bool
srec_to_loadimage (uint8_t *buf, size_t sz, uint8_t *image, size_t image_sz)
{
  uint8_t *p;
  size_t i;

  __srec_load_base = image;
  __srec_load_limit = image_sz;

  srec_init ();
  for (i = 0, p = buf; i < sz; p++, i++)
    {
      srec_return_t ret;
      if ((ret = srec_parser (*p)) != SREC_OK)
	{
	  if (ret != SREC_EOF)
	    {
	      fprintf (stderr, "Error. %s\n", srec_error_message[ret]);
	      return FALSE;
	    }
	  break;
	}
    }
  fprintf (stderr, "%ld byte. start addr=%x\n", i, srec_start_addr);

  return TRUE;
}