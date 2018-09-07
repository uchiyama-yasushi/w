all: ram

ROOT_DIR	= ../../../

include ../../mk/7785.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o
KERNOBJS += ${KERN_THREAD_OBJS} ${KERN_MONITOR_OBJS} ${KERN_RINGBUF_OBJS}

#-DTHREAD_DEBUG 
CFLAGS += -DDEBUG -DTHREAD_VERBOSE -DRAM_CHECK_ENABLE -DSTATIC=
CFLAGS += -DPROMPT="\"test4> \"" -DPROMPT_BUFFER_SIZE=64 -DPROMPT_HISTORY_CONF=2
CFLAGS += -DSH4A_EXT_ADDR32

ASFLAGS +=

OBJS = entry.o main.o ${KERNOBJS} cpu.o cpu_subr.o exception_generic.o	\
exception_tlbmiss.o exception_interrupt.o exception_reset.o		\
exception.o start.o fpu_test.o ram_check.o console_machdep.o		\
serial_console.o cache.o pmb.o pmb_subr.o mmu.o fpool.o vm.o pager.o	\
cpg.o interrupt.o

LDSCRIPT = ${LDSCRIPT_DIR}/sh7785lcr.ram

LIBTYPE = MINLIB

include ${MK_DIR}/target.mk
