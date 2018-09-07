ROOT_DIR	= ../../..

all:	ram

ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk
CFLAGS += -DTHREAD_DISABLE -DSTATIC=

VPATH += ${ROOT_DIR}/common
OBJS = overlay_test.o

PROGTYPE = OVERLAY
IMPORT_SYMBOL = ../h8_monitor/h8_monitor.elf

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk