include ${ROOT_DIR}/mk/conf.mk

CPU_DIR		= ${ROOT_DIR}/sh4a
MK_DIR		= ${CPU_DIR}/mk
LDSCRIPT_DIR	= ${CPU_DIR}/ldscripts

INCLUDES = -I${ROOT_DIR}/include
INCLUDES += -I${ROOT_DIR}/minlib/include
INCLUDES += -I${CPU_DIR}/include
INCLUDES += -I${CPU_DIR}/${CPUARCH} -I${CPU_DIR}/${SHMODEL}
INCLUDES += -I${CPU_DIR}

VPATH += ${ROOT_DIR}/kernel/
VPATH += ${CPU_DIR}/${CPUARCH}
VPATH += ${CPU_DIR}/${SHMODEL}
