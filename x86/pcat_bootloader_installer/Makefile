ROOT_DIR= ../..

include ../mk/pcat.mk

OBJS_KERNEL = gdt.o start.o installer.o
# x86
OBJS_KERNEL += mode16_longcall_subr.o mode16_longcall.o	\
bios_bounce_buffer.o cpu.o
# PC/AT
OBJS_KERNEL += nmi.o pckbc.o  bios_service.o bios_console_vga.o	\
bios_console_serial.o
# kernel
OBJS_KERNEL += console.o bios_console.o ${KERN_SHELL_OBJS}	\
 ${KERN_THREAD_OBJS} ${KERN_CORE_OBJS} shell_command.o
# BFS
OBJS_KERNEL += bfs.o bfs_init.o sysvbfs_partition_find.o	\
bfs_bios.o
# Disk I/O
OBJS_KERNEL += pcat_mbr.o bios_int13_rw.o bios_int13_rw_ext.o	\
bios_fdd.o bios_hdd.o

OBJS_REALMODE = mode16_longcall_subr.o bios_console_vga.o	\
		bios_console_serial.o bios_int13_rw.o		\
		bios_int13_rw_ext.o bios_service.o		\
		bios_bounce_buffer.o

#
PROGS		= kernel

MINLIB	= ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	= ${MINLIB}


FLAGS += -DMONITOR_DISABLE -DREALMODE_CS=0x8000 -DBOOTSTRAP_IDT	\
-DBIOS_BOUNCE_BUFFER -DPROMPT="\"install> \""			\
-DTHREAD_STACKSIZE_DEFAULT=1024
#
FLAGS += -DSTATIC=static -DDEBUG
#FLAGS +=  -DBIOSIO_DEBUG -DBFS_DEBUG
#FLAGS +=  -DTHREAD_VERBOSE -DTHREAD_DEBUG

all:	${PROGS}

install: install_hdd install_floppy

kernel: descriptor_memory ${OBJS_KERNEL} ${OBJS_REALMODE}
	${LD} -T ${LDSCRIPT_DIR}/installer.ldscript -o $@.elf -e machine_startup ${OBJS_KERNEL} ${LIBS}
	${OBJCOPY} -I elf32-i386 -O binary $@.elf $@

clean::
	-rm -f ${OBJS_BOOT1} ${OBJS_KERNEL} ${PROGS} *.elf *.core gdt.s gdt.h descriptor_memory boot

install_hdd: all
	vnconfig vnd0 ${DISKIMAGE}
	mount_sysvbfs -o nodev,nosuid /dev/vnd0f /mnt
	chown -R ${USER}:${GROUP} /mnt
	cp kernel.elf /mnt
	umount /mnt
	vnconfig -u vnd0

install_floppy: all
	vnconfig vnd0 ${FLOPPYIMAGE}
	mount_sysvbfs -o nodev,nosuid /dev/vnd0a /mnt
	chown -R ${USER}:${GROUP} /mnt
	cp kernel.elf /mnt
	umount /mnt
	vnconfig -u vnd0
