ROOT_DIR = ../../../
include ${ROOT_DIR}/mk/conf.mk

SHELL	= /bin/sh
GNUARCH	= h8300-elf

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

CFLAGS += -msx -fomit-frame-pointer -Wall -Werror
ASFLAGS = -msx -Wall -Werror

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

OBJS	= entry.o
PROG	= test.elf
PROGBASENAME = ${basename ${PROG}}

all:	${PROG}

clean:
	rm -f ${OBJS} ${PROGBASENAME}.*
	rm -rf ${DEPEND_DIR}

romwrite: all
	../../h8sxwrite/h8sxwrite -f 12 -d 0 -m 0 ${PROGBASENAME}.mot

${PROG}:	${OBJS}
	${LD} -T ldscript -o ${PROG}  ${OBJS}
	${OBJDUMP} -x ${PROG}
	${OBJDUMP} -D ${PROG}
	${OBJCOPY} -Osrec  $@ ${PROGBASENAME}.mot

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
