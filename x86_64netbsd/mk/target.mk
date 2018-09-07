PROGBASENAME = ${notdir ${PWD}}

PROG = ${PROGBASENAME}.elf
LDSCRIPT ?= ${LDSCRIPT_DIR}/elf_x86_64.x

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}

ifneq ($(LIBTYPE),NOLIB)
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	+= ${MINLIB} -lgcc
else
LIBS	+= -lc -lgcc
endif
endif

all:	${PROG}

# crtbegin.o, crtend,o: .ctors .dtors
# crti.o: _init, _fini
# crtn.o: .init, .fini

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -entry=_start ${OBJS} ${LIBS} -o ${PROGBASENAME}.elf

