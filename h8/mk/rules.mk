SHELL	= /bin/sh

PREFIX	= ${TOOLDIR}/bin/${GNUARCH}

AS	= ${PREFIX}-gcc
CPP	= ${PREFIX}-cpp
CC	= ${PREFIX}-gcc
C++	= ${PREFIX}-g++
LD	= ${PREFIX}-ld
OBJCOPY	= ${PREFIX}-objcopy
OBJDUMP	= ${PREFIX}-objdump
NM	= ${PREFIX}-nm
AR	= ${PREFIX}-ar
RANLIB	= ${PREFIX}-ranlib


DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P

ASFLAGS	= ${H8MODE_FLAG}

CFLAGS += ${H8MODE_FLAG} -nostdinc -fno-builtin
#
#-ffreestanding 
CFLAGS += -fomit-frame-pointer -mquickcall -mrelax
CFLAGS += -Wall -Wextra -Werror

ifeq ($(TARGET_OPT),noopt)
# -O causes compiler bug. (lua)
else
CFLAGS += -O
endif

CFLAGS += ${INCLUDES}

ifeq ($(H8MODEL),3694)
# H8/3694
	CFLAGS += -DSYSTEM_CLOCK=20
endif
ifeq ($(H8MODEL),3664)
# H8/3664
	CFLAGS += -DSYSTEM_CLOCK=16
endif
ifeq ($(H8MODEL),3052)
# H8/3052
	CFLAGS += -DSYSTEM_CLOCK=25
endif


C++FLAGS = -fno-rtti -fno-access-control -fno-threadsafe-statics

VPATH += ${PWD}

.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
#	${C++} ${CFLAGS} ${C++FLAGS} ${DEPEND_UPDATE} -c -o $@ $<

.cpp.o:
	${C++} ${CFLAGS} ${C++FLAGS} ${DEPEND_UPDATE} -c -o $@ $<

check:
	${OBJDUMP} -x ${PROGBASENAME}.elf
	${NM} -n ${PROGBASENAME}.elf > ${PROGBASENAME}.nm
	${OBJDUMP} -D ${PROGBASENAME}.elf > ${PROGBASENAME}.dis

clean::
	rm -f *.o *.elf *.mot *.nm *.dis TAGS
	rm -rf ${DEPEND_DIR}

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P


