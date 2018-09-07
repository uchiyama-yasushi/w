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

CFLAGS += -nostdinc -fno-builtin
CFLAGS += -fomit-frame-pointer -Wall -Wextra -Werror ${INCLUDES}
CFLAGS += -g
ASFLAGS = -Wall -Werror ${INCLUDES}

.c.o:
	${CC} ${INCLUDES} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

check:
	${OBJDUMP} -x ${PROGBASENAME}.elf
	${OBJDUMP} -D ${PROGBASENAME}.elf

clean::
	rm -f ${OBJS}
	rm -f *.elf *.mot *.nm *.dis TAGS *.bin *.hex
	rm -rf ${DEPEND_DIR}

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P
DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
