ROOT_DIR	= ../../..

#all:	rom
all:	ram
ram:
	PROGRAM=RAM ${MAKE} build

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ${ROOT_DIR}/common
VPATH += ../libud01
VPATH += ../ud01

# for standalone.

H8OBJS += sci1_eri.o sci1_rxi.o timer_itu.o
KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_FIBER_OBJS} ${KERN_MONITOR_OBJS} ${KERN_TIMER_OBJS}	\
	${KERN_SHELL_OBJS} ${KERN_RINGBUF_OBJS}				\
	${KERN_BUFCONSOLE_OBJS} shell_command.o bitdisp.o		\
	nmi_monitor.o nmi.o

UD01_OBJS = sg12232_init.o sg12232_export.o sg12232_font.o

OBJS += ${UD01_OBJS} fiber_test.o fiber_main.o
CFLAGS += -DSTATIC=

ifeq ($(PROGRAM),ROM)
ENTRYOBJ = entry_ud01.o	# override default.
OBJS += ${KERNOBJS} ${H8OBJS} board_ud01_device.o
else
OBJS += ${KERNOBJS} ${H8OBJS} board_ud01_device.o
LDSCRIPT = ${LDSCRIPT_DIR}/ud01.ram
#IMPORT_SYMBOL = ../ud01_monitor/ud01_monitor.elf	#for overlay
endif

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
