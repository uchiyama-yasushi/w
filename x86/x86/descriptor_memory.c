
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

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef int8_t bool;
#include <segment.h>
#include <gdt_config.h>	// Application specific setting.

int
main ()
{
  char header[] = "// THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.";
  union descriptor descriptor;
  struct gdt_config *conf = gdt_config;
  struct application_descriptor *d = &descriptor.desc;
  int32_t limit;
  int i, j, n, selector;
  FILE *gdt_s = fopen ("gdt.s", "w");
  FILE *gdt_h = fopen ("gdt.h", "w");

  assert (gdt_s && gdt_h);
  fprintf (gdt_s, "%s\n", header);
  fprintf (gdt_h, "%s\n", header);


  fprintf (gdt_s, "\t.arch i486\n");

  // GDT
  fprintf (gdt_s, "\t.section .data\n\t.balign 8\n");
  fprintf (gdt_s, "\t.global _gdt\n_gdt:\n");

  // Null Descriptor #0
  fprintf (gdt_s, "//     base size unit mode type\n");
  fprintf (gdt_s, "//[0x00] Null descriptor\n");
  for (j = 0; j < 4; j++)
    fprintf (gdt_s, "\t.short 0x0000\n");

  n = sizeof gdt_config / sizeof (gdt_config[0]);
  // Application Descriptor. (code, data, stack)
  for (i = 0, selector = 8; i < n; i++, conf++, selector += 8)
    {
      fprintf (gdt_h, "#define\tGDT_%s\t0x%x\n", conf->name, selector);
      fprintf (gdt_h, "#define\tGDT_%s_BASE\t0x%x\n", conf->name,
	       conf->base);

      memset (d, 0, sizeof *d);
      fprintf (gdt_s, "// [0x%x] %x %x %02x %02x %02x %s\n", selector,
	       conf->base, conf->size, conf->unit, conf->mode, conf->type,
	       conf->name);
      d->access_byte	= conf->type;
      d->base_0_15	= conf->base & 0xffff;
      d->base_16_23	= (conf->base >> 16) & 0xff;
      d->base_24_31	= (conf->base >> 24) & 0xff;

      limit = conf->size;
      if (conf->type == STACK_SEGMENT)
	{
	  limit = -limit;
	}
      limit--;
      d->size_0_15	= limit & 0xffff;
      //      printf (">>%x %x %x\n", ((limit >> 16) & 0xf) , conf->unit , conf->mode);
      d->size_16_19_GD	= ((limit >> 16) & 0xf) | conf->unit | conf->mode;
      for (j = 0; j < 4; j++)
	fprintf (gdt_s, "\t.short 0x%04x\n", descriptor.u16[j]);
    }

  // Argument for LGDT
  fprintf (gdt_s, "\t.balign 4\n");
  fprintf (gdt_s, "\t.global\tlgdt_arg\nlgdt_arg:\n");
  fprintf (gdt_s, "\t.word\t0x%x\n\t.long\t_gdt\n",
	   sizeof descriptor * (n + 1) - 1);

  fclose (gdt_s);

  return 0;
}
