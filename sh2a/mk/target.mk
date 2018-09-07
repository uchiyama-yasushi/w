PROGBASENAME = ${notdir ${PWD}}

ifeq ($(PROGRAM),ROM)
# Don't override user specified ldscript.
	LDSCRIPT ?= ${LDSCRIPT_DIR}/7262.rom
	PROG = ${PROGBASENAME}.mot
endif

ifeq ($(PROGRAM),ROM_MONITOR)
	LDSCRIPT ?= ${LDSCRIPT_DIR}/spirom
#	LDFLAGS = --retain-symbols-file ${ROOT_DIR}/include/export_symbols
	PROG_TYPE = binary
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),RAM)
	CFLAGS += -DRAM_PROGRAM
	LDSCRIPT ?= ${LDSCRIPT_DIR}/7262.ram
	PROG = ${PROGBASENAME}.mot
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	CFLAGS += -DRAM_PROGRAM
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	LDFLAGS = -R ${IMPORT_SYMBOL_PROG}
	PROG = ${PROGBASENAME}.mot
endif

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	+= ${MINLIB} -lgcc
else
LIBS	+= -lc -lgcc
endif

build:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -entry=_start -o ${PROGBASENAME}.elf  ${OBJS} ${LIBS}
ifeq ($(PROGTYPE),binary)
	${OBJCOPY} -Obinary ${PROGBASENAME}.elf $@
else
	${OBJCOPY} -Osrec ${PROGBASENAME}.elf $@
	cp $@ a.mot
endif
