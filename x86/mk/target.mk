PROGBASENAME = ${notdir ${PWD}}

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	+= ${MINLIB} -lgcc
else
# use newlib.
LIBS	+= -lc -lgcc
endif

PROG = ${PROGBASENAME}.elf
all:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} ${LDFLAGS} -T ${LDSCRIPT}  ${CRTOBJS} ${OBJS} ${LIBS} -o ${PROG}
	${MAKE_FSELF} ${PROG} ${PROGBASENAME}.self


