ROOT_DIR = ../../..
include ${ROOT_DIR}/mk/conf.mk
SHELL	= /bin/sh
GNUARCH	= sh-elf

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

CFLAGS += -ml -m4a  -Wall -Werror
ASFLAGS = -ml -m4a  -Wall -Werror

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

${PROG}:	${OBJS}
	${LD} -T ../../ldscripts/test.ram -o ${PROG}  ${OBJS}
	${OBJDUMP} -x ${PROG}
	${OBJDUMP} -D ${PROG}
	${OBJCOPY} -I elf32-shl -O binary ${PROG} ${PROGBASENAME}.bin
	cp test.bin /tftpboot/sh7785.img

#	hexdump -C ${PROGBASENAME}.bin

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
