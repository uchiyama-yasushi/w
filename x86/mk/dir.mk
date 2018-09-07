
CPU_DIR		= ${ROOT_DIR}/x86
MK_DIR		= ${CPU_DIR}/mk
include ${MK_DIR}/arch.mk

LDSCRIPT_DIR	= ${ROOT_DIR}/${CPUARCH}/ldscripts

INCLUDES = -I${ROOT_DIR}/include -I${ROOT_DIR}/minlib/include		\
-I${ROOT_DIR}/${CPUARCH}/include -I${ROOT_DIR}/${CPUARCH}/${MACHINE}	\
-I${ROOT_DIR}/filesystem -I.

VPATH += .
VPATH += ${ROOT_DIR}/kernel
VPATH += ${ROOT_DIR}/filesystem
VPATH += ${ROOT_DIR}/filesystem/sysvbfs
VPATH += ${ROOT_DIR}/${CPUARCH}/${CPUARCH}
VPATH += ${ROOT_DIR}/${CPUARCH}/${MACHINE}
VPATH += ${ROOT_DIR}/${CPUARCH}/${MACHINE}/dev
VPATH += ${ROOT_DIR}/common

NEWLIB_DIR = ${TOOLDIR}/${GNUARCH}/lib
GCCLIB_DIR = ${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}

# These variable can override.
DISKIMAGE ?= ${ROOT_DIR}/${CPUARCH}/pcat_diskimage/disk.img
FLOPPYIMAGE ?= ${ROOT_DIR}/${CPUARCH}/pcat_floppyimage/floppy.img