all:	rom_monitor
#all:	ram

ROOT_DIR	= ../../../

include ../../mk/7262.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DBROKEN_SH2A_MONITOR
CFLAGS += -DSTATIC=static -DDEBUG
CFLAGS += -DTHREAD_DISABLE
CFLAGS += -DPROMPT="\"mon> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += ${CFLAGS}

OBJS = entry.o main.o ${KERNOBJS} ${H8SXOBJS} srec.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk

