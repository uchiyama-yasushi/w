
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

#ifndef _LOCAL_H_
#define	_LOCAL_H_

__BEGIN_DECLS
// Root thread.
extern thread_t shell_th;

// Logger status.
enum log_status
  {
    LOG_STATUS_IDLE,
    LOG_STATUS_SAMPLING,
    LOG_STATUS_STOP,
  };
extern enum log_status log_status_flag;

// DIP switch
extern bool dipsw_test0;
extern bool dipsw_test1;
extern bool dipsw_laptime;
extern int dipsw_nmagnet;
void controller_init (void);
void controller_dipswitch_update (void);

bool logger_start (void);
void logger_stop (void);

extern int current_log_number;

#ifdef DEBUG
#define	DPRINTF(fmt, args...)	iprintf("%s: " fmt, __FUNCTION__, ##args)
#else
#define	DPRINTF(fmt, args...)	((void)0)
#endif

__END_DECLS
#endif
