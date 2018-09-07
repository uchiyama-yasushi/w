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

CFLAGS +=  -nostdinc -fno-builtin
CFLAGS += -mcpu=arm7tdmi-s -O -Wall -Wextra -Werror ${INCLUDES}
ASFLAGS = -mcpu=arm7tdmi-s -Wall -Werror ${INCLUDES}

ifeq ($(TARGET_OPT),noopt)
else
CFLAGS += -O
endif

.c.o:
	${CC} ${INCLUDES} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

check:
	${OBJDUMP} -x ${PROGBASENAME}.elf
	${OBJDUMP} -D ${PROGBASENAME}.elf
	hexdump -C ${PROGBASENAME}.bin

clean::
	rm -f exception_vector26.o exception_vector32.o
	rm -f ${OBJS}
	rm -f *.elf *.mot *.nm *.dis TAGS *.bin *.hex
	rm -rf ${DEPEND_DIR}

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P
DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
