# program for loaded by ../simple_monitor
ram:
	PROGRAM=RAM ${MAKE} build
ram_overlay:
	PROGRAM=RAM_OVERLAY ${MAKE} build
# ROM program.
rom:
	PROGRAM=ROM ${MAKE} build
rom_monitor:
	PROGRAM=ROM_MONITOR ${MAKE} build

CPUARCH		= sh2a
SH2AMODEL	= 7262
GNUARCH		= sh-elf

include ${ROOT_DIR}/sh2a/mk/dir.mk
include ${MK_DIR}/rules.mk
include ${MK_DIR}/objs.mk
include ${ROOT_DIR}/filesystem/filesystem.mk