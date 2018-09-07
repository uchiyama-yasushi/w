
ROOT_DIR	= ../../../

include ../../mk/host.mk


CFLAGS += -DSTATIC= -DDEBUG -DTHREAD_DISABLE
ASFLAGS += ${CFLAGS}
CFLAGS += -I${ROOT_DIR}/lua/src

#VPATH	+= ${ROOT_DIR}/lua/src
#OBJS += ${subst .c,.o, ${notdir ${shell ls ${ROOT_DIR}/lua/src/*.c}}}

OBJS += ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
shell_command.o main.o lua.o fiber_subr.o stream_mem.o netbsd_glue.o start.o cpu_subr.o

LIBTYPE = MINLIB
LIBS += ${CPU_DIR}/liblua/liblua.a
LIBS += -lm

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk
