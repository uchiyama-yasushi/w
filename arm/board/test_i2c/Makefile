all: ram

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_BUFCONSOLE_OBJS}		\
${KERN_SHELL_OBJS} ${KERN_THREAD_OBJS} ${KERN_MONITOR_OBJS}	\
${KERN_FIBER_OBJS} ${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS}	\
${KERN_RINGBUF_OBJS} shell_command.o

CFLAGS += -DDEBUG  -DTHREAD_VERBOSE  -DSTATIC=""
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DPROMPT="\"i2c> \"" -DPROMPT_BUFFER_SIZE=32 -DPROMPT_HISTORY_CONF=4

OBJS = vic.o clock_config.o main.o ${KERNOBJS} ${ARMOBJS}
OBJS += gpio.o i2c.o kxp84_i2c.o

glut:
	cc -I/usr/X11R7/include glut_test.c -R/usr/X11R7/lib -L/usr/X11R7/lib -lglut -lGLU -lGL -lXmu -lXi -lXext -lX11 -lm -lpthread
LIBTYPE = MINLIB
include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk

clean::
	rm -f a.out
