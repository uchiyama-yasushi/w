ROOT_DIR= ../..

include ../mk/pcat.mk

#
# !!! real-mode object is located to kernel.ldscript directly
#
OBJS_REALMODE = mode16_longcall_subr.o bios_console_vga.o	\
		bios_console_serial.o bios_int13_rw.o		\
		bios_int13_rw_ext.o bios_service.o		\
		bios_bounce_buffer.o

OBJS_KERNEL = gdt.o mode16_longcall_subr.o bios_console_vga.o		\
bios_int13_rw.o bios_int13_rw_ext.o bios_fdd.o bios_hdd.o		\
bios_service.o bios_console_serial.o bios_bounce_buffer.o	\

OBJS_KERNEL += mode16_longcall.o font_clR8x8.o start.o board.o	\
console.o bios_console.o ${KERN_SHELL_OBJS} shell_command.o	\
${KERN_THREAD_OBJS} cpu.o cpu_subr.o ${KERN_CORE_OBJS}		\
${KERN_MONITOR_OBJS} descriptor_gate.o idt.o bitdisp.o		\
${KERN_RINGBUF_OBJS} ${KERN_FIBER_OBJS} vector.o vector_table.o	\
pckbc.o i8259.o local_apic.o nmi.o ram_check.o random.o

OBJS_KERNEL += bfs.o bfs_init.o sysvbfs_partition_find.o bfs_bios.o
OBJS_KERNEL += pcat_mbr.o
OBJS_KERNEL += tss.o descriptor.o panic.o callgate.o callergate.o calleegate.o
OBJS_KERNEL += exception_vector.o exception_vector_table.o	\
exception_pcat.o task.o fiber_test.o
OBJS_KERNEL += vga_console.o clock.o keyboard.o

#
PROGS		= kernel

FLAGS = -DSERIAL_CONSOLE
#FLAGS = -DGRAPHIC_CONSOLE
FLAGS += -DRAM_CHECK_ENABLE -DREALMODE_CS=0x8000 -DBIOS_BOUNCE_BUFFER
#
FLAGS += -DSTATIC=static -DDEBUG
FLAGS +=  -DBIOSIO_DEBUG -DBFS_DEBUG
CFLAGS +=  -DTHREAD_VERBOSE -DTHREAD_DEBUG

MINLIB	= ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	= ${MINLIB}

all:	${PROGS}

install: install_floppy install_hdd

kernel: descriptor_memory ${OBJS_KERNEL} ${OBJS_REALMODE}
	${LD} -T ${LDSCRIPT_DIR}/kernel.ldscript -o $@.elf -e machine_startup ${OBJS_KERNEL} ${LIBS}
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

test:
	${MAKE} -f ../pcat_diskimage/Makefile test
# type "load 0 kernel.elf"