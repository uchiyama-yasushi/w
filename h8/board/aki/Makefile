ROOT_DIR= ../../../

all:	rom

ram:
	PROGRAM=RAM PROGTYPE=OVERLAY ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk
include ${MK_DIR}/objs.mk

CFLAGS += -DDEBUG -DTHREAD_DISABLE -DSHELL_DISABLE -DSTATIC=

VPATH += ../libaki

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS}
AKI_OBJS = board_aki.o
AKI_OBJS += aki_lcd.o aki_led.o aki_switch.o

OBJS = ${H8OBJS} ${KERNOBJS} ${AKI_OBJS}
ifeq ($(PROGRAM),ROM)
OBJS += ${KERN_CONSOLE_OBJS}
endif

IMPORT_SYMBOL = ../h8_monitor/h8_monitor.elf #for overlay
LIBTYPE=MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk