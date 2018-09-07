SHELL	= /bin/sh

CC	= gcc

CFLAGS		= -Wall -Wextra -Werror -O2
DEPEND_DIR	= .deps
DEPEND_UPDATE	= -Wp,-MD,$(DEPEND_DIR)/$(*F).P

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<

all:	lpc23xxwrite lpc23xxuud

OBJS	= main.o lpc23xx_isp.o uuencode.o lpc23xx_vector_fix.o

lpc23xxwrite:	${OBJS}
	${CC} -o $@ ${OBJS}

lpc23xxuud:	uudecode.o
	${CC} -o $@ uudecode.o

clean:
	rm -f ${OBJS} lpc23xxwrite lpc23xxuud uudecode.o
	rm -rf ${DEPEND_DIR}

check:	lpc23xxuud
	./lpc23xxuud tmp.uu tmp.bin
	arm-elf-objcopy -I binary -O elf32-littlearm tmp.bin tmp.elf
	arm-elf-objdump -m arm -D tmp.elf

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
