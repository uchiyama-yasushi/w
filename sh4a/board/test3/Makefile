all: ram

ROOT_DIR	= ../../../

include ../../mk/7785.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DDEBUG -DTHREAD_DISABLE -DVM_DISABLE -DSTATIC=
CFLAGS += -DPROMPT="\"test3> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += -DTHREAD_DISABLE -DVM_DISABLE

OBJS =  entry.o main.o test.o ${KERNOBJS} cpu.o cpu_subr.o exception_generic.o exception_tlbmiss.o exception_interrupt.o exception.o cpg.o interrupt.o
LDSCRIPT = ${LDSCRIPT_DIR}/sh7785lcr.ram.P2

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
