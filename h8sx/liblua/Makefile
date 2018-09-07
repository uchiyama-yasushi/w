
ROOT_DIR	= ../..

LIB	= liblua.a
all:	${LIB}
TARGET_OPT = lua

include ../mk/1655.mk

VPATH	+= ${ROOT_DIR}/lua/src
OBJS = ${subst .c,.o, ${notdir ${shell ls ${ROOT_DIR}/lua/src/*.c}}}

${LIB}: ${OBJS}
	rm -f ${LIB}
	${AR} cq ${LIB} ${OBJS}
	${RANLIB} ${LIB}

clean::
	rm -f ${LIB}
