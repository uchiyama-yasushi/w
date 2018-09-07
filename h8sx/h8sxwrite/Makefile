SHELL	= /bin/sh

CC		= gcc
CFLAGS		= -Wall -Wextra -Werror -O2
#CFLAGS	+= -DDEBUG -g
DEPEND_DIR	= .deps
DEPEND_UPDATE	= -Wp,-MD,$(DEPEND_DIR)/$(*F).P

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<


all:	h8sxwrite

OBJS	= bootmode_interface.o bootmode_core.o h8sx.o main.o srec.o

h8sxwrite:	${OBJS}
	${CC} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} h8sxwrite
	rm -rf ${DEPEND_DIR}

DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P
