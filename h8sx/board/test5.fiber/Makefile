
all:	ram

ROOT_DIR	= ../../../

include ../../mk/1655.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DSTATIC=static
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DTHREAD_VERBOSE -DDEBUG -DSCI_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
CFLAGS += -DEXT_FRAME_INFO
ASFLAGS += ${CFLAGS}

VPATH += ${ROOT_DIR}/common

OBJS = ${KERNOBJS} ${H8SXOBJS} entry.o mcu.o clock.o		\
interrupt.o power.o vector.o intc.o tmr.o thread.o	\
thread_stack.o thread_machdep.o thread_subr.o monitor.o			\
console_machdep.o sci4.o rbuf_nolock.o serial_console.o ringbuffer.o	\
frame.o
OBJS += ${KERN_FIBER_OBJS} fiber_test.o main.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk