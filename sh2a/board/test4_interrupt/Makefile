
all:	ram
#all:	rom

ROOT_DIR	= ../../../

include ../../mk/7262.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DSTATIC= -DDEBUG
CFLAGS += -DTHREAD_DISABLE
CFLAGS += -DINTC_DEBUG -DPWR_DEBUG -DCLK_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += ${CFLAGS}

OBJS = ${KERNOBJS} ${SH2AOBJS} entry.o main.o intc.o clk.o pwr.o	\
bitdisp.o vector.o mcu.o interrupt.o irq3.o frame.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
