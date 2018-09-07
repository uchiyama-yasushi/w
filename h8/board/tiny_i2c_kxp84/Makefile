ROOT_DIR	= ../../..

all:
	PROGRAM=RAM ${MAKE} build

#include ../../mk/3694.mk
include ../../mk/3664.mk

include ${MK_DIR}/objs.mk

#
#CFLAGS += -DDEBUG
#CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=384
#CFLAGS += -DDELAY_CALIBRATE_DISABLE
CFLAGS += -DSTATIC=
CFLAGS += -DTHREAD_DISABLE
#CFLAGS += -DSHELL_DISABLE
#CFLAGS += -DSMALL_PRINTF
#CFLAGS += -DBUFFERD_CONSOLE_DISABLE
#CFLAGS += -DTHREAD_DEBUG -DTHREAD_VERBOSE
#PROMPT_BUFFER_SIZE
#PROMPT_HISTORY_MAX
#SHELL_CMDARG_MAX
#THREAD_PRIORITY_MAX

LIBTYPE = MINLIB
OBJS = tiny_i2c_kxp84_overlay.o i2c_intr.o i2c.o wkp_intr.o

PROGTYPE = OVERLAY
ifeq ($(H8MODEL),3694)
	IMPORT_SYMBOL = ../tiny3694_monitor/tiny3694_monitor.elf
else
	IMPORT_SYMBOL = ../tiny3664_monitor/tiny3664_monitor.elf
endif

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
