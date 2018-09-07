
ROOT_DIR	= ../../../

include ../../mk/host.mk


CFLAGS += -DSTATIC= -DDEBUG -DTHREAD_DISABLE
ASFLAGS += ${CFLAGS}

OBJS += ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
shell_command.o main.o cpu_subr.o

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
