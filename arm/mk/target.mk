PROGBASENAME = ${notdir ${PWD}}

ifeq ($(PROGRAM),ROM)
	OBJS += exception_vector26.o
# Don't override user specified ldscript.
	LDSCRIPT ?= ${LDSCRIPT_DIR}/rom.504KB
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),ROM_MONITOR)
	OBJS += exception_vector26.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/rom.monitor
	LDFLAGS = --retain-symbols-file ${ROOT_DIR}/kernel/include/export_symbols
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),RAM)
	CFLAGS += -DRAM_PROGRAM
	OBJS += exception_vector32.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	PROG = ${PROGBASENAME}.mot
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	CFLAGS += -DRAM_PROGRAM
	OBJS += exception_vector32.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	LDFLAGS = -R ${IMPORT_SYMBOL_PROG}
	PROG = ${PROGBASENAME}.mot
endif

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS +=  ${MINLIB} -lgcc
else
# use newlib.
LIBS += -lc -lgcc
endif

build:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -o ${PROGBASENAME}.elf  ${OBJS} ${LIBS}
ifeq ($(PROGRAM),ROM)
	${OBJCOPY} -Obinary ${PROGBASENAME}.elf $@
endif
ifeq ($(PROGRAM),ROM_MONITOR)
	${OBJCOPY} -Obinary ${PROGBASENAME}.elf $@
endif
ifeq ($(PROGRAM),RAM)
	${OBJCOPY} -Osrec ${PROGBASENAME}.elf $@
	cp $@ a.mot
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	${OBJCOPY} -Osrec ${PROGBASENAME}.elf $@
	cp $@ a.mot
endif

