include ${ROOT_DIR}/mk/conf.mk

H8_DIR		= ${ROOT_DIR}/h8
CPU_DIR		= ${ROOT_DIR}/h8
MK_DIR		= ${H8_DIR}/mk
LDSCRIPT_DIR	= ${H8_DIR}/ldscripts

INCLUDES = -I${ROOT_DIR}/include
INCLUDES += -I${ROOT_DIR}/minlib/include
INCLUDES += -I${H8_DIR}/include -I${H8_DIR}/${H8ARCH}
INCLUDES += -I${H8_DIR}/board

VPATH += ${ROOT_DIR}/kernel/
VPATH += ${H8_DIR}/h8
VPATH += ${H8_DIR}/${H8ARCH}

ifeq ($(H8CPUMODE),normal)
GCCLIB_DIR =${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}/h8300h/normal
NEWLIB_DIR =${TOOLDIR}/${GNUARCH}/lib/h8300h/normal
else # advanced mode
GCCLIB_DIR =${TOOLDIR}/lib/gcc/${GNUARCH}/${GCC_VERSION}/h8300h
NEWLIB_DIR =${TOOLDIR}/${GNUARCH}/lib/h8300h
endif
