
LIB	= minlib.a
all:	${LIB}

MINLIB_DIR = ${ROOT_DIR}/minlib
VPATH	+= ${MINLIB_DIR}/ctype
VPATH	+= ${MINLIB_DIR}/divmod
VPATH	+= ${MINLIB_DIR}/errno
VPATH	+= ${MINLIB_DIR}/locale
VPATH	+= ${MINLIB_DIR}/stdio
VPATH	+= ${MINLIB_DIR}/stdlib
VPATH	+= ${MINLIB_DIR}/string
VPATH	+= ${MINLIB_DIR}/unistd

OBJS = ${subst .c,.o, ${notdir ${shell ls ${MINLIB_DIR}/*/*.c}}}

${LIB}: ${OBJS}
	rm -f ${LIB}
	${AR} cq ${LIB} ${OBJS}
	${RANLIB} ${LIB}

clean::
	rm -f ${LIB} ${OBJS}
