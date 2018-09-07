ROOT_DIR	= ../../..

all:
	PROGRAM=RAM ${MAKE} build

#include ../../mk/3694.mk
include ../../mk/3664.mk

include ${MK_DIR}/objs.mk

#
CFLAGS += -DDEBUG -DSTATIC=static
#CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=256
#CFLAGS += -DDELAY_CALIBRATE_DISABLE
CFLAGS += -DTHREAD_DISABLE
#CFLAGS += -DSHELL_DISABLE
#CFLAGS += -DSMALL_PRINTF
#CFLAGS += -DBUFFERD_CONSOLE_DISABLE
#CFLAGS += -DTHREAD_DEBUG -DTHREAD_VERBOSE
#PROMPT_BUFFER_SIZE
#PROMPT_HISTORY_MAX
#SHELL_CMDARG_MAX
#THREAD_PRIORITY_MAX

VPATH += ${ROOT_DIR}/common
LIBTYPE	= MINLIB
#OBJS = overlay_test.o
OBJS = tiny_overlay.o  timer_a_test.o timer_a.o

PROGTYPE = OVERLAY
ifeq ($(H8MODEL),3694)
	IMPORT_SYMBOL = ../tiny3694_monitor/tiny3694_monitor.elf
else
	IMPORT_SYMBOL = ../tiny3664_monitor/tiny3664_monitor.elf
endif

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
