ROOT_DIR	= ../../..

all:	rom
rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ${ROOT_DIR}/common
VPATH += ../libud01

#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps@25MHz
CFLAGS += -DTHREAD_DISABLE -DSTATIC=
CFLAGS += -DPROMPT="\">> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
CFLAGS += -DSHELL_NOBUILTIN_COMMAND

ENTRY_OBJ = entry_ud01.o	# ud01 special.
KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_SHELL_OBJS}	\
	${KERN_FIBER_OBJS} ${KERN_CONSOLE_OBJS}

PROGTYPE = MONITOR
LIBTYPE = MINLIB

LDSCRIPT = ${LDSCRIPT_DIR}/ud01.rom_monitor
OBJS = ${H8OBJS} ${KERNOBJS} board_ud01_minmonitor.o

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk