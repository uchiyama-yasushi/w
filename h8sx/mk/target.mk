PROGBASENAME = ${notdir ${PWD}}

ifeq ($(PROGRAM),ROM)
#	OBJS += exception_vector26.o
# Don't override user specified ldscript.
	LDSCRIPT ?= ${LDSCRIPT_DIR}/1655.rom
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),ROM_MONITOR)
#	OBJS += exception_vector26.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/1655.rom_monitor
	LDFLAGS = --retain-symbols-file ${ROOT_DIR}/kernel/include/export_symbols
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),RAM)
	CFLAGS += -DRAM_PROGRAM
#	OBJS += exception_vector32.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/1655.ram
	PROG = ${PROGBASENAME}.mot
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	CFLAGS += -DRAM_PROGRAM
#	OBJS += exception_vector32.o
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	LDFLAGS = -R ${IMPORT_SYMBOL_PROG}
	PROG = ${PROGBASENAME}.mot
endif

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS += ${MINLIB} -lgcc
else
# use newlib.
LIBS += -lc -lm -g -lgcc
endif

build:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -o ${PROGBASENAME}.elf  -L${GCCLIB_DIR} -L${NEWLIB_DIR} ${OBJS} ${LIBS} -lgcc
	${OBJCOPY} -Osrec ${PROGBASENAME}.elf $@
	cp $@ a.mot

