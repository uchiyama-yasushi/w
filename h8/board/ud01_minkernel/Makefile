ROOT_DIR	= ../../..

all:	ram

ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01

vpath
vpath %.c ../ud01
CFLAGS	+= -DTHREAD_DISABLE -DLCD_DISABLE -DSHELL_DISABLE -DRAM_CHECK_ENABLE
CFLAGS += -DDEBUG -DSTATIC=
OBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_CONSOLE_OBJS}	\
	board_ud01_minkernel.o board_ud01_device.o ram_check.o		\
	${H8OBJS}

IMPORT_SYMBOL = ../h8_monitor/h8_monitor.elf
LIBTYPE = MINLIB
#IMPORT_SYMBOL = ../ud01_monitor/ud01_monitor.elf

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk