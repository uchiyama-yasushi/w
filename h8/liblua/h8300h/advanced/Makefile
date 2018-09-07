
ROOT_DIR	= ../../../../

LIB	= liblua.a
all:	${LIB}
TARGET_OPT = noopt

include ../../../mk/3052.mk

VPATH	+= ${ROOT_DIR}/lua/src
OBJS = ${subst .c,.o, ${notdir ${shell ls ${ROOT_DIR}/lua/src/*.c}}}

${LIB}: ${OBJS}
	${AR} cq ${LIB} ${OBJS}
	${RANLIB} ${LIB}

clean::
	rm -f ${LIB}
