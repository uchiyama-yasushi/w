
ROOT_DIR	= ../../../
include ${ROOT_DIR}/mk/conf.mk

PREFIX	= x86_64-netbsd-elf
CC	= ${PREFIX}-gcc
LD	= ${PREFIX}-ld

.c.o:
	${CC} -nostdinc -fno-builtin -c -o $@ $<

OBJS = main.o

LIBTYPE = NOLIB

all: ${OBJS}
	${LD} -T ldscript -entry=_start ${OBJS}

clean:
	rm -f ${OBJS} a.out
