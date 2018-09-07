ROOT_DIR= ../../../
include ${ROOT_DIR}/mk/conf.mk
SHELL	= /bin/sh
GNUARCH	= arm-elf

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

CFLAGS += -mcpu=arm7tdmi-s -fomit-frame-pointer -Wall -Werror
ASFLAGS = -mcpu=arm7tdmi-s -fomit-frame-pointer -Wall -Werror

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

OBJS	= entry.o main.o
PROG	= test.elf
PROGBASENAME = ${basename ${PROG}}

all:	${PROG}

clean:
	rm -f ${OBJS} ${PROGBASENAME}.*
	rm -rf ${DEPEND_DIR}

romwrite:
	../../lpc23xxwrite/lpc23xxwrite ${PROGBASENAME}.bin

${PROG}:	${OBJS}
	${LD} -T ../../ldscripts/rom.4KB -o ${PROG}  ${OBJS}
	${OBJCOPY} -Oihex ${PROG} ${PROGBASENAME}.hex
	${OBJDUMP} -x ${PROG}
	${OBJDUMP} -D ${PROG}
	${OBJCOPY} -I ihex -O binary ${PROG} ${PROGBASENAME}.bin
	hexdump -C ${PROGBASENAME}.bin

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
