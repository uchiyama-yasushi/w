
all:	rom_monitor

ROOT_DIR	= ../../../

include ../../mk/1655.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DSTATIC=static
CFLAGS += -DTHREAD_DISABLE
CFLAGS += -DPROMPT="\"mon> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += ${CFLAGS}

OBJS = ${KERNOBJS} ${H8SXOBJS} simple_monitor.o entry.o srec.o	\
srec_load.o clock.o mcu.o vector.o tmr.o interrupt.o intc.o power.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk