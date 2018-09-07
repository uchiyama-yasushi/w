
all: ram
#all: rom

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_BUFCONSOLE_OBJS}		\
${KERN_SHELL_OBJS} ${KERN_THREAD_OBJS} ${KERN_MONITOR_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} ${KERN_RINGBUF_OBJS}	\
shell_command.o

#ACCELEROMETER = 1
#DISTANCEMETER = 1
#GPS = 1

ifdef ACCELEROMETER
CFLAGS += -DENABLE_ACCELEROMETER
endif
ifdef DISTANCEMETER
CFLAGS += -DENABLE_DISTANCEMETER
endif
ifdef GPS
CFLAGS += -DENABLE_GPS
endif
CFLAGS += -DDEBUG
#CFLAGS += -DTHREAD_VERBOSE
#CFLAGS += -DTHREAD_DEBUG
CFLAGS += -DPROMPT="\"log> \"" -DPROMPT_BUFFER_SIZE=32 -DPROMPT_HISTORY_CONF=4

#CFLAGS += -DSTATIC=static
CFLAGS += -DSTATIC=

# Kernel and ARM core.
OBJS = clock_config.o ${KERNOBJS} ${ARMOBJS} gpio.o vic.o
# SD card
OBJS += mci.o dma.o mci_dma.o mci_fsops.o
# Filesystem
OBJS += bfs_simple.o ${BFS_CORE_OBJS} ${BFS_DATAFILE_OBJS} ${PCAT_MBR_OBJS}
# RTC
OBJS += rtc.o
ifdef GPS
# UART GPS-52
OBJS += uart.o gps.o ${KERN_FIBER_OBJS}
endif
# SPI LCD-module
OBJS += spi.o lcd_display.o lcd_vram.o lcd_app.o
# Logger itself
OBJS += main.o storage.o data.o controller.o timecnt.o
ifdef ACCELEROMETER
# Accelerometer KXP84
OBJS += accelerometer.o kxp84_i2c.o i2c.o
endif
# Comparator D/A Converter provides P-LAP signal threshold voltage.
OBJS += dac.o
ifdef DISTANCEMETER
# Distance measuring module GP2Y0A21
OBJS += distancemeter.o adc.o
endif

LIBTYPE = MINLIB
#LIBTYPE = NEWLIB

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk

hostprog:
	cc -Wall -Werror check.c

clean::
	rm -f a.out
