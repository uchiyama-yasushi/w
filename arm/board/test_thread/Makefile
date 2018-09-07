all: ram

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_BUFCONSOLE_OBJS}		\
${KERN_SHELL_OBJS} ${KERN_THREAD_OBJS} ${KERN_MONITOR_OBJS}	\
${KERN_FIBER_OBJS} ${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS}	\
${KERN_RINGBUF_OBJS} shell_command.o

CFLAGS += -DDEBUG  -DTHREAD_VERBOSE -DSTATIC=""
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=32 -DPROMPT_HISTORY_CONF=4

VPATH += ${ROOT_DIR}/common

OBJS = vic.o clock_config.o main.o ${KERNOBJS} fiber_test.o gpio.o \
${ARMOBJS}
LIBTYPE = MINLIB
include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk