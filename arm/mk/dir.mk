include ${ROOT_DIR}/mk/conf.mk

CPU_DIR		= ${ROOT_DIR}/arm
MK_DIR		= ${CPU_DIR}/mk
LDSCRIPT_DIR	= ${CPU_DIR}/ldscripts

INCLUDES = -I${ROOT_DIR}/include
INCLUDES += -I${ROOT_DIR}/minlib/include
INCLUDES += -I${CPU_DIR}/include
INCLUDES += -I${CPU_DIR}/${CPUARCH} -I${CPU_DIR}/${ARMMODEL}
INCLUDES += -I${ROOT_DIR}
INCLUDES += -I${CPU_DIR}

VPATH += ${ROOT_DIR}/kernel/
VPATH += ${CPU_DIR}/${CPUARCH}
VPATH += ${CPU_DIR}/${ARMMODEL}
VPATH += ${ROOT_DIR}/device/
VPATH += ${CPU_DIR}/device/

NEWLIB_DIR	= ${TOOLDIR}/${GNUARCH}/lib
GCCLIB_DIR	= ${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}

SERIAL_DEVICE	= /dev/dtyU0
