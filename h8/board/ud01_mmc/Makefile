ROOT_DIR	= ../../..

all:	ram

ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01
VPATH += ../ud01
VPATH += ${ROOT_DIR}/device
VPATH += ${ROOT_DIR}/filesystem
VPATH += ${ROOT_DIR}/filesystem/sysvbfs
CFLAGS += -I${ROOT_DIR}/filesystem

CFLAGS += -DDEBUG -DBFS_DEBUG -DLCD_DISABLE -DSTATIC=
#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps
#CFLAGS += -DPROCESS_MODE

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_MONITOR_OBJS} ${KERN_BUFCONSOLE_OBJS}			\
	${KERN_SHELL_OBJS} shell_command.o ${KERN_TIMER_OBJS}		\
	${KERN_RINGBUF_OBJS}

UD01_OBJS = board.o mmc_spi.o mmc_machdep.o bfs.o	\
bfs_init.o sysvbfs_partition_find.o pcat_mbr.o

OBJS = ${UD01_OBJS}

ifeq ($(PROGRAM),ROM)
OBJS += ${KERNOBJS} ${H8OBJS} board_ud01_device.o
else
PROGTYPE = OVERLAY
LDSCRIPT = ${LDSCRIPT_DIR}/ud01.ram
endif

IMPORT_SYMBOL = ../ud01_monitor/ud01_monitor.elf
LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk