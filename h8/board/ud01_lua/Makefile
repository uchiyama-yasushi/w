ROOT_DIR	= ../../..

all:
	PROGRAM=RAM ${MAKE} build
TARGET_OPT = lua
include ../../mk/3052.mk

include ${MK_DIR}/objs.mk

VPATH += ../libud01
VPATH += ../ud01

#CFLAGS += -DSCI_SPEED_BRR=13  #57600bps
UD01_OBJS =
#UD01_OBJS += push_switch.o push_switch_rfshc_cmi.o
CFLAGS += -DDEBUG -DSTATIC=

PROGTYPE = OVERLAY
OBJS = ${UD01_OBJS}
LDSCRIPT = ${LDSCRIPT_DIR}/ud01.ram.luaoverlay

CFLAGS += -I${ROOT_DIR}/lua/src

#LIBS += ${CPU_DIR}/liblua/h8300h/advanced/liblua.a -lm
#OBJS += main.o lua.o stream_mem.o fiber_subr.o
OBJS += main.o lua.o
# 
IMPORT_SYMBOL = ../ud01_luamonitor/ud01_luamonitor.elf
LIBTYPE=MINLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk