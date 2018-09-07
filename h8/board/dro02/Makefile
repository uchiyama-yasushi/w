all:	rom

ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

ROOT_DIR	= ../../..

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

CFLAGS += -I${ROOT_DIR}/device/dro -I${PWD} -DSTATIC=
#CFLAGS += -DDEBUG
CFLAGS += -DBUFFERD_CONSOLE_DISABLE -DSHELL_DISABLE

VPATH += ${ROOT_DIR}/dro02 ${ROOT_DIR}/device/dro

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_MONITOR_OBJS} ${KERN_RINGBUF_OBJS} ${KERN_CONSOLE_OBJS}

DRO02OBJS = board_dro02.o
DRO02OBJS += keyboard16.o keyboard16_rfshc.o led_display.o
DRO02OBJS += caliper_manager.o caliper_terminal.o caliper_config.o
DRO02OBJS += caliper_common.o shan.o shan_irq0.o shan_irq1.o shan_irq2.o \
	isagida.o isagida_irq3.o isagida_c_irq3.o

OBJS = ${H8OBJS} ${KERNOBJS} ${DRO02OBJS}
LIBTYPE=MINLIB
include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk