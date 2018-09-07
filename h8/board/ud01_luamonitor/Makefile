ROOT_DIR	= ../../..

all:	rom

rom:
	PROGRAM=ROM ${MAKE} build

include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01

CFLAGS += -DDEBUG -DSTATIC=
#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=1024 -DRAM_CHECK_ENABLE
CFLAGS += -DPROMPT="\">> \""

ENTRY_OBJ = entry_ud01.o	# ud01 special.
H8OBJS +=  sci1_eri.o sci1_rxi.o timer_itu.o
KERNOBJS = ${KERN_CORE_OBJS} ${KERN_DELAY_OBJS} ${KERN_THREAD_OBJS}	\
	${KERN_FIBER_OBJS} ${KERN_MONITOR_OBJS} ${KERN_SHELL_OBJS}	\
	${KERN_TIMER_OBJS} ${KERN_RINGBUF_OBJS}				\
	${KERN_BUFCONSOLE_OBJS} shell_command.o
KERNOBJS += bitdisp.o nmi_monitor.o nmi.o ram_check.o
UD01_OBJS = board_ud01_monitor.o board_ud01_device.o trapa0.o
UD01_OBJS += sg12232_init.o sg12232_export.o sg12232_font.o
OBJS = ${H8OBJS} ${KERNOBJS} ${UD01_OBJS}

CFLAGS += -I${ROOT_DIR}/lua/src
LIBS += ${CPU_DIR}/liblua/h8300h/advanced/liblua.a -lm
OBJS += lua.o stream_mem.o
OBJS += ${shell ls ${CPU_DIR}/minlib/h8300h/advanced/*.o}

LDSCRIPT = ${LDSCRIPT_DIR}/ud01.rom_monitor
PROGTYPE = MONITOR
LIBTYPE=NOLIB
LIBS += -lm -lgcc

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk