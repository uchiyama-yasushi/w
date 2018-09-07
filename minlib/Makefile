SHELL	= /bin/sh
GNUARCH	= sh-elf

CFLAGS += -Iinclude -I../include -I/usr/include
CFLAGS += -g
CFLAGS += -DSELFTEST -DSTATIC= -nostdinc -no-builtin-function -ffreestanding -Wall
#CFLAGS +=  -Werror
#ASFLAGS = -m2a -Wall -Werror


.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<
.S.o:
	${AS} ${INCLUDES} ${ASFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

VPATH += ../kernel
OBJS	= stream_mem.o format_scan.o format_print.o
PROG	= test.elf
PROGBASENAME = ${basename ${PROG}}

all:	${PROG}

clean:
	rm -f ${OBJS}

${PROG}:	${OBJS}
	${CC}  -o ${PROG}  ${OBJS}
