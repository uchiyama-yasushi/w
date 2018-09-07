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

CPUARCH		= sh4a
SHMODEL		= 7785
GNUARCH		= sh-elf

# -mdivsi3_libfunc=__divsi3_i4i
CPUFLAGS	= -m4a -ml
NEWLIB_DIR	= ${TOOLDIR}/${GNUARCH}/lib/ml/m4
GCCLIB_DIR	= ${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}/ml/m4
AS_CPUFLAGS	= -Wa,-isa=sh4a

# No-FPU See gcc-4.3.2/gcc/config/sh/embed-elf.h
#CPUFLAGS	= -m2 -ml
#NEWLIB_DIR	= ${TOOLDIR}/${GNUARCH}/lib/ml/m2
#GCCLIB_DIR	= ${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}/ml/m2
#EXTLIB		= -lgcc-Os-4-200
#AS_CPUFLAGS	= -Wa,-isa=sh2

include ${ROOT_DIR}/sh4a/mk/dir.mk
include ${MK_DIR}/rules.mk
include ${MK_DIR}/objs.mk
include ${ROOT_DIR}/filesystem/filesystem.mk