ROOT_DIR	= ../../..

all:
	PROGRAM=RAM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01
VPATH += ../ud01

#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps
UD01_OBJS = board_ud01.o
#UD01_OBJS += push_switch.o push_switch_rfshc_cmi.o
CFLAGS += -DDEBUG -DSTATIC=

PROGTYPE = OVERLAY
OBJS = ${UD01_OBJS}
LDSCRIPT = ${LDSCRIPT_DIR}/ud01.ram

IMPORT_SYMBOL = ../ud01_monitor/ud01_monitor.elf
LIBTYPE=MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk