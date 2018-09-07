all: ram

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_BUFCONSOLE_OBJS}		\
${KERN_SHELL_OBJS} ${KERN_THREAD_OBJS} ${KERN_MONITOR_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS}	\
${KERN_RINGBUF_OBJS} shell_command.o

CFLAGS += -DDEBUG  -DTHREAD_VERBOSE -DSTATIC=""
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DPROMPT="\"user> \"" -DPROMPT_BUFFER_SIZE=32 -DPROMPT_HISTORY_CONF=4

VPATH += ${ROOT_DIR}/common
VPATH += ${ROOT_DIR}/filesystem/v7fs

OBJS = vic.o clock_config.o main.o ${KERNOBJS} gpio.o \
${ARMOBJS} mci.o dma.o mci_dma.o mci_fsops.o

OBJS += v7fs.o v7fs_datablock.o v7fs_dirent.o v7fs_endian.o	\
v7fs_file.o v7fs_inode.o v7fs_io.o v7fs_superblock.o

#v7fs_io_user.o 

LIBTYPE = MINLIB
include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk