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

CPUARCH		= arm
ARMMODEL	= lpc23xx
GNUARCH		= arm-elf

include ${ROOT_DIR}/arm/mk/dir.mk
include ${MK_DIR}/rules.mk
include ${MK_DIR}/objs.mk
include ${ROOT_DIR}/filesystem/filesystem.mk