all:	rom

rom:
	PROGRAM=ROM ${MAKE} build

ROOT_DIR	= ../../..

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

CFLAGS += -I${ROOT_DIR}/device/dro -I${PWD}
CFLAGS += -DCALIPER_SHELL_COMMAND -DSTATIC=static
#CFLAGS += -DDEBUG
CFLAGS += -DSCI_SPEED_BRR=13  #57600bps

VPATH += ${ROOT_DIR}/dro01 ${ROOT_DIR}/device/dro ../libaki

H8OBJS += sci1_eri.o sci1_rxi.o timer_itu.o
KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_MONITOR_OBJS} ${KERN_BUFCONSOLE_OBJS}			\
	${KERN_SHELL_OBJS} shell_command.o ${KERN_TIMER_OBJS}		\
	${KERN_RINGBUF_OBJS}

DRO01OBJS = board_dro01.o
DRO01OBJS += caliper_manager.o caliper_terminal.o caliper_config.o
DRO01OBJS += caliper_common.o shan.o shan_irq0.o shan_irq1.o shan_irq2.o
DRO01OBJS += aki_lcd.o

OBJS = ${H8OBJS} ${KERNOBJS} ${DRO01OBJS}
LIBTYPE=MINLIB
include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk