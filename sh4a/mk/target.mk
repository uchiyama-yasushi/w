PROGBASENAME = ${notdir ${PWD}}

ifeq ($(PROGRAM),ROM)
# Don't override user specified ldscript.
	LDSCRIPT ?= ${LDSCRIPT_DIR}/rom.504KB
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),ROM_MONITOR)
	LDSCRIPT ?= ${LDSCRIPT_DIR}/rom.monitor
	LDFLAGS = --retain-symbols-file ${ROOT_DIR}/include/export_symbols
	PROG = ${PROGBASENAME}.bin
endif

ifeq ($(PROGRAM),RAM)
	CFLAGS += -DRAM_PROGRAM
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	PROG = ${PROGBASENAME}.elf
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	CFLAGS += -DRAM_PROGRAM
	LDSCRIPT ?= ${LDSCRIPT_DIR}/ram
	LDFLAGS = -R ${IMPORT_SYMBOL_PROG}
	PROG = ${PROGBASENAME}.elf
endif

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifeq ($(LIBTYPE),MINLIB)
MINLIB = ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	+= ${MINLIB} -lgcc-Os-4-200
#-lgcc-Os-4-200 ... udivsi3_i4i
else
# use libgcc and newlib.
LIBS	+= -lc -lgcc-Os-4-200
endif

build:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -o ${PROGBASENAME}.elf  ${OBJS} ${LIBS} ${MINLIB}
ifeq ($(PROGRAM),ROM)
	${OBJCOPY} -Obinary ${PROGBASENAME}.elf $@
endif
ifeq ($(PROGRAM),ROM_MONITOR)
	${OBJCOPY} -Obinary ${PROGBASENAME}.elf $@
endif
ifeq ($(PROGRAM),RAM)
	${OBJCOPY} -I elf32-shl -O binary ${PROG} ${PROGBASENAME}.bin
	cp ${PROGBASENAME}.bin /tftpboot/sh7785.img
endif
ifeq ($(PROGRAM),RAM_OVERLAY)
	${OBJCOPY} -I elf32-shl -O binary ${PROG} ${PROGBASENAME}.bin
	cp ${PROGBASENAME}.bin /tftpboot/sh7785.img
endif

