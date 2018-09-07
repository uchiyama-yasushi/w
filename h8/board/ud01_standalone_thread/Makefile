ROOT_DIR	= ../../..

#all:	rom
all:	ram
ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01
VPATH += ../ud01

CFLAGS += -DDEBUG -DTHREAD_VERBOSE -DSTATIC=
#CFLAGS += -DTHREAD_DEBUG
#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps

H8OBJS += sci1_eri.o sci1_rxi.o timer_itu.o
KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_MONITOR_OBJS} ${KERN_TIMER_OBJS} ${KERN_SHELL_OBJS}	\
	${KERN_RINGBUF_OBJS} ${KERN_BUFCONSOLE_OBJS} shell_command.o

UD01_OBJS = board_ud01.o

CFLAGS += -DLCD_DISABLE
#sg12232_init.o sg12232_export.o sg12232_font.o nmi_restart.o

OBJS = ${UD01_OBJS}

ifeq ($(PROGRAM),ROM)
ENTRYOBJ = entry_ud01.o	# override default.
OBJS += ${KERNOBJS} ${H8OBJS} board_ud01_device.o
else
OBJS += ${KERNOBJS} ${H8OBJS} board_ud01_device.o
LDSCRIPT = ${LDSCRIPT_DIR}/ud01.ram
#IMPORT_SYMBOL = ../ud01_monitor/ud01_monitor.elf
endif

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk