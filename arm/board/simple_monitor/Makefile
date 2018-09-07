#
# AND SEE README
#
# To write LPC2388 flash, type 'gmake romwrite'
# Check SERIAL_DEVICE.
# If displayed
# '?' read failed. PLEASE RESET AGAIN.: Resource temporarily unavailable
# Short LPC2388 reset pin.
#
# /etc/remote:
# umon230400:dv=/dev/dtyU0:br#230400:pa=none:dc:
#
# Change to target directory.
# $ tip umon230400
# connected
# mon> 
# mon> help
# avaliable command: help reset l 
# mon> 
# mon> l
# Send S-record file.
# ~>Local file name? a.mot
# 6320 lines transferred in 6 seconds 
# !
# Read 148459 byte. success
# Start address: 0x40003d70
# stack_start: 0x4000f800
# RAM data: 0x4000bfd0-0x4000c718 1864byte
# bss: 0x4000c720-0x4000e288 7016byte
# current stack=0x4000f7f8
# ...


all:	rom_monitor

ROOT_DIR	= ../../../

include ../../mk/lpc2388.mk

KERNOBJS = ${KERN_CORE_OBJS} ${KERN_CONSOLE_OBJS} ${KERN_SHELL_OBJS}	\
${KERN_DELAY_OBJS} ${KERN_TIMER_OBJS} shell_command.o

CFLAGS += -DSTATIC=static
CFLAGS += -DTHREAD_DISABLE
CFLAGS += -DPROMPT="\"mon> \"" -DPROMPT_BUFFER_SIZE=16 -DPROMPT_HISTORY_CONF=0
ASFLAGS += ${CFLAGS}

OBJS = simple_monitor.o srec.o srec_load.o gpio.o ${KERNOBJS} ${ARMOBJS}

LIBTYPE = MINLIB

############################
SERIAL_DEVICE	= /dev/dtyU0
############################

include ${MK_DIR}/target.mk
include ${MK_DIR}/utils.mk