
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

#ifndef _DATA_H_
#define	_DATA_H_
// Logger Data format.
/*
   31       27                         0
  |T |datatype|    current time (msec)   |
  | trailer data (if any                 |
  T bit is trailers bit.
 */

#define	DATA_TYPE_MASK		0x1f
#define	DATA_TYPE_SHIFT		27

#define	DATA_TYPE(x)	((x) & (DATA_TYPE_MASK << DATA_TYPE_SHIFT))
#define	DATA_TIME(x)	((x) & ~(DATA_TYPE_MASK << DATA_TYPE_SHIFT))
#define	DATA_TYPE_INTR(x) (!((x) & (DATA_TYPE_TRAILERS << DATA_TYPE_SHIFT)))

// External interrupt can be multiplexed.(trailer data is its current time only)


#define	DATA_TYPE_INTR_SPEED	(0x01 << DATA_TYPE_SHIFT)
#define	DATA_TYPE_INTR_LAP	(0x02 << DATA_TYPE_SHIFT)
#define	DATA_TYPE_INTR_RPM	(0x04 << DATA_TYPE_SHIFT)
#define	DATA_TYPE_INTR_MISC	(0x08 << DATA_TYPE_SHIFT)
#define	DATA_LEN_INTR		0
// These data types are not multiplexed.
#define	DATA_TYPE_TRAILERS	0x10
#define	DATA_TYPE_ACCELEROMETER	((DATA_TYPE_TRAILERS | 0x0) << DATA_TYPE_SHIFT)
#define	DATA_LEN_ACCELEROMETER	1
#define	DATA_TYPE_STROKE_F	((DATA_TYPE_TRAILERS | 0x1) << DATA_TYPE_SHIFT)
#define	DATA_LEN_STROKE_F	1
#define	DATA_TYPE_STROKE_R	((DATA_TYPE_TRAILERS | 0x2) << DATA_TYPE_SHIFT)
#define	DATA_LEN_STROKE_R	1
#define	DATA_TYPE_GEAR		((DATA_TYPE_TRAILERS | 0x3) << DATA_TYPE_SHIFT)
#define	DATA_LEN_GEAR		1
#define	DATA_TYPE_THROTTLE	((DATA_TYPE_TRAILERS | 0x4) << DATA_TYPE_SHIFT)
#define	DATA_LEN_THROTTLE	1
#define	DATA_TYPE_BRAKE_F	((DATA_TYPE_TRAILERS | 0x5) << DATA_TYPE_SHIFT)
#define	DATA_LEN_BRAKE_F	1
#define	DATA_TYPE_BRAKE_R	((DATA_TYPE_TRAILERS | 0x6) << DATA_TYPE_SHIFT)
#define	DATA_LEN_BRAKE_R	1
#define	DATA_TYPE_GPS		((DATA_TYPE_TRAILERS | 0x7) << DATA_TYPE_SHIFT)
#define	DATA_LEN_GPS		3

#define	DATA_TYPE_ERROR		((DATA_TYPE_TRAILERS | 0xf) << DATA_TYPE_SHIFT)
#define	DATA_LEN_ERROR		0

// End mark.
#define	DATA_TYPE_END		0xf8000000

__BEGIN_DECLS
#ifndef STANDALONE
extern thread_t data_th;
thread_t data_init (void);
void data_event_trailers (uint32_t, uint32_t *, int);
int data_current_lap (void);
int data_current_speed (void);
#endif
__END_DECLS


// My machine specific data
// Rear tire circumference.
#define	CIRCUMFERENCE	188	// cm

#endif
