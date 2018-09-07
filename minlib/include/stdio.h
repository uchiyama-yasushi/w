
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

#ifndef _STDIO_H_
#define	_STDIO_H_
#include <sys/types.h>
#define	BUFSIZ	1024		// setbuf, setvbuf buffer size.
#define	EOF	(-1)

struct _file;

typedef struct _file
{
  bool active;
  uint32_t flags;
#define	_IOEOF	0x010
#define	_IOERR	0x020
  //Buffering flags.
#define	_IOFBF	0x040
#define	_IOLBF	0x080
#define	_IONBF	0x100

  struct _stream_char_ops *char_ops;
  struct _stream_line_ops *line_ops;
  struct _stream_binary_ops *binary_ops;
} FILE;

extern struct _file *stdin, *stdout, *altin, *altout;
#define	stderr	altout

#include <sys/stream.h>
#include <stdarg.h>
void format_print (struct _stream_char_ops *, const char *, va_list);
int format_scan (struct _stream_char_ops *, const char *, va_list);

FILE *fopen (const char *, const char *);
int fclose (FILE *);
FILE *freopen (const char *, const char *, FILE *);

/* Character stream ops. */
int ungetc (int, FILE *);
int fgetc (FILE *);
#define	getc(f)	fgetc(f)
int fputc (int, FILE *);

/* Line stream ops. */
char *fgets (char *, int, FILE *);
int fputs (const char *, FILE *);

/* Binary stream ops. */
size_t fread (void *, size_t, size_t, FILE *);
size_t fwrite (const void *, size_t, size_t, FILE *);

/* Format I/O ops. */
int fprintf (FILE *, const char *, ...);
int fscanf (FILE *, const char *, ...);

/* Stream position ops. */
long ftell (FILE *);
int fseek (FILE *, long, int);
#define	SEEK_SET	0
#define	SEEK_CUR	1
#define	SEEK_END	2

/* Buffering ops. */
int fflush (FILE *);
int setvbuf (FILE *, char *, int, size_t);
/* Error handling */
#define	feof(p)		((p)->flags & _IOEOF)
#define	ferror(p)	((p)->flags & _IOERR)
#define	clearerr(p)	((p)->flags &= ~(_IOERR | _IOEOF))

/* File ops. */
int remove (const char *);
int rename (const char *, const char *);
FILE *tmpfile (void);
char *tmpnam (char *);
#define	L_tmpnam	8	//tmpnam filename length.

int sprintf (char *, const char *, ...);
int snprintf (char *, size_t, const char *, ...);
int vsprintf (char *, const char *, va_list);
int vsnprintf (char *, size_t, const char *, va_list);

// During interrupt context, use these.
#define	iprintf(fmt, args...)	fprintf (altout, fmt, ##args)
#define	printf(fmt, args...)	fprintf (stdout, fmt, ##args)
#define	getchar()		fgetc (stdin)
#endif	//!_STDIO_H_
