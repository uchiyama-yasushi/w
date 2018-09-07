
#all:	ram
all:	rom

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DSTATIC= -DDEBUG
CFLAGS += -DTHREAD_DISABLE
CFLAGS += -DINTC_DEBUG -DPWR_DEBUG -DCLK_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += ${CFLAGS}
CFLAGS += -I${ROOT_DIR}/lua/src

VPATH	+= ${ROOT_DIR}/lua/src
LIBS += ${CPU_DIR}/liblua/liblua.a -lm
#OBJS += ${subst .c,.o, ${notdir ${shell ls ${ROOT_DIR}/lua/src/*.c}}}

OBJS += vic.o clock_config.o main.o ${KERNOBJS} lua.o	\
stream_mem.o fiber_subr.o ${ARMOBJS}

LIBTYPE = MINLIB


include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
