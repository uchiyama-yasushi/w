
# objects for targets.
ENTRY_OBJ ?= entry_no_bsc_init.o
MONITOR_OBJS = ${ENTRY_OBJ} start.o vector_virtual.o			\
vector_virtual_link.o vector_virtual_update.o srec_load.o srec.o
OVERLAY_OBJS = entry_overlay.o start.o vector.o
STANDALONE_OBJS = entry_no_bsc_init.o start.o vector.o

ifeq ($(PROGRAM),ROM)
# ROM-monitor
	ifeq ($(PROGTYPE),MONITOR)
		OBJS += ${MONITOR_OBJS}
		LDSCRIPT ?= ${LDSCRIPT_DIR}/${H8ARCH}.rom_monitor
# ROM-monitor export symbols.
		LDFLAGS = --retain-symbols-file ${ROOT_DIR}/kernel/include/export_symbols
	else
# ROM-program
		OBJS += ${STANDALONE_OBJS}
# Don't override user specified ldscript.
		LDSCRIPT ?= ${LDSCRIPT_DIR}/${H8ARCH}.rom
	endif
endif
ifeq ($(PROGRAM),RAM)
	ifeq ($(PROGTYPE),OVERLAY)
		LDFLAGS = -R ${IMPORT_SYMBOL}
		CFLAGS += -DOVERLAY
	endif
	CFLAGS += -DRAM_PROGRAM
	LDSCRIPT ?= ${LDSCRIPT_DIR}/${H8ARCH}.ram
	OBJS += ${OVERLAY_OBJS}
endif
ifeq ($(PROGRAM),TEST)
	LDSCRIPT ?= ${LDSCRIPT_DIR}/${H8ARCH}.ram
endif

PROG = ${notdir ${PWD}}.mot
PROGBASENAME = ${basename ${PROG}}

LIBS += -L${NEWLIB_DIR} -L${GCCLIB_DIR}
ifneq ($(LIBTYPE),NOLIB)
ifeq ($(LIBTYPE),MINLIB)
	MINLIB = ${CPU_DIR}/minlib/h8300h/${H8CPUMODE}/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib/h8300h/${H8CPUMODE}
LIBS += ${MINLIB} -lgcc
else
# use newlib.
LIBS += -lc -lm -g -lgcc
endif
endif

ifeq ($(PROGTYPE),OVERLAY)
LIBS = ${MINLIB}
endif


build:	${PROG}

${PROG}: ${OBJS} ${MINLIB}
	${LD} -T ${LDSCRIPT} ${LDFLAGS} -o ${PROGBASENAME}.elf  ${OBJS} ${LIBS}
	${OBJCOPY} -Osrec ${PROGBASENAME}.elf $@
	cp $@ a.mot
