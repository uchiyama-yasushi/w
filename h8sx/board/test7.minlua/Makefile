
all:	rom

ROOT_DIR	= ../../../

include ../../mk/1655.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
 shell_command.o interrupt.o clock.o

CFLAGS += -DSTATIC=static
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DTHREAD_DISABLE -DDEBUG -DSCI_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
CFLAGS += -DEXT_FRAME_INFO
ASFLAGS += ${CFLAGS}

VPATH += ${ROOT_DIR}/common

OBJS = ${KERNOBJS} ${H8SXOBJS} entry.o power.o vector.o

#OBJS += mcu.o intc.o tmr.o thread.o thread_stack.o thread_machdep.o thread_subr.o monitor.o console_machdep.o sci4.o rbuf_nolock.o serial_console.o ringbuffer.o frame.o

CFLAGS += -DBUFFERD_CONSOLE_DISABLE -DPOLLING_PROGRAM
CFLAGS += -I${ROOT_DIR}/lua/src
LIBS += ${CPU_DIR}/liblua/liblua.a -lm
OBJS += ${KERN_FIBER_OBJS} main.o stream_mem.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk