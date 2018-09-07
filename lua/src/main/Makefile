ROOT_DIR	= ../../../
VPATH += ${ROOT_DIR}/lua/src

CORE_OBJS = ${subst .c,.o, ${notdir ${shell ls ${ROOT_DIR}/lua/src/*.c}}}

#lmathlib.o 

LUA_OBJS = lua.o
LUAC_OBJS = luac.o print.o

#OBJS =  malloc2.o fiber_subr.o

CFLAGS += -I${ROOT_DIR}/lua/src/
CFLAGS += -g

DEPEND_DIR	=	.deps
DEPEND_UPDATE	=	-Wp,-MD,$(DEPEND_DIR)/$(*F).P

.c.o:
	${CC} ${CFLAGS} ${DEPEND_UPDATE} -c -o $@ $<


all:	lua_host luac_host

clean:
	rm -f ${OBJS} ${LUA_OBJS} ${LUAC_OBJS} ${CORE_OBJS} lua_host luac_host
	rm -rf ${DEPEND_DIR}

lua_host:	${OBJS} ${LUA_OBJS} ${CORE_OBJS}
	${CC} -o lua_host ${OBJS} ${LUA_OBJS} ${CORE_OBJS} -lm

luac_host:	${OBJS} ${LUAC_OBJS} ${CORE_OBJS}
	${CC} -o luac_host ${OBJS} ${LUAC_OBJS} ${CORE_OBJS} -lm


DEPS_MAGIC := ${shell mkdir ${DEPEND_DIR} > /dev/null 2>&1 || :}
-include ${DEPEND_DIR}/*.P


