SHELL	= /bin/sh

include ${ROOT_DIR}/mk/conf.mk
include ${MK_DIR}/dir.mk
include ${MK_DIR}/objs.mk

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

LIBS	= ${NEWLIB} ${GCCLIB}
#LIBS	=

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P

CFLAGS += -nostdinc -fno-builtin
CFLAGS += -Os -fomit-frame-pointer ${INCLUDES} ${FLAGS}
CFLAGS += -Wall -Werror

ASFLAGS	= ${INCLUDES} ${FLAGS}


.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

.c.o:
	${CC} ${INCLUDES} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

gdt.s:	${ROOT_DIR}/${CPUARCH}/include/${MACHINE}/gdt_config.h
	./descriptor_memory

gdt.h:	${ROOT_DIR}/${CPUARCH}/include/${MACHINE}/gdt_config.h
	./descriptor_memory

gdt.o:	gdt.s gdt.h
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ gdt.s

descriptor_memory: ${ROOT_DIR}/${CPUARCH}/include/${MACHINE}/gdt_config.h
	cc ${DESCRIPTOR_FLAGS} ${ROOT_DIR}/${CPUARCH}/${CPUARCH}/descriptor_memory.c \
	-I${ROOT_DIR}/${CPUARCH}/include/${CPUARCH} \
	-I${ROOT_DIR}/${CPUARCH}/include/${MACHINE} -o $@


DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P

clean::
	rm -f ${OBJS}
	rm -rf ${DEPEND_DIR}
	rm -f *.core
