ROOT_DIR= ../..

include ../mk/pcat.mk

OBJS_BOOT1 = boot1.o gdt.o mode32.o bios_console_vga.o	\
bios_console_util.o bios_int13_rw.o

OBJS_BOOT2 = boot2.o start.o mode16.o
OBJS_BOOT2 += console.o bios_console.o bios_console_vga.o	\
bios_console_serial.o
OBJS_BOOT2 += assert.o cpu.o a20.o pckbc.o
OBJS_BOOT2 += bfs.o bfs_init.o sysvbfs_partition_find.o
OBJS_BOOT2 += bios_fdd.o bios_int13_rw.o bios_service.o
OBJS_BOOT2 += load_elf.o
OBJS_BOOT2 += bios_delay.o bios_delay_subr.o bios_timer.o bios_timer_subr.o
#
#
#
PROGS	= boot1 boot2
DESCRIPTOR_FLAGS = -DBOOTSTRAP_GDT

FLAGS	= -DSTATIC="" -DTHREAD_DISABLE -DBOOTSTRAP_IDT -DBOOTSTRAP_GDT
#FLAGS	+= -DSERIAL_CONSOLE
FLAGS	+= -DDEBUG
# -DBFS_DEBUG -DBIOSIO_DEBUG
#----------------------------------------------------------------------
MINLIB	= ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	= ${MINLIB}

all:	${PROGS}
	dd if=/dev/zero of=${FLOPPYIMAGE} bs=18k count=80
	vnconfig vnd0 ${FLOPPYIMAGE}
	disklabel -w -f floppy.disklabel vnd0 floppy
	newfs_sysvbfs /dev/rvnd0a
#	mount -o nodev,nosuid /dev/vnd0a /mnt
#	chown -R ${USER}:${GROUP} /mnt
#	cp ../pcat_kernel/kernel.elf /mnt
#	umount /mnt
	vnconfig -u vnd0
	dd if=boot1 of=${FLOPPYIMAGE} bs=512 count=1 conv=notrunc
	dd if=boot2 of=${FLOPPYIMAGE} bs=512 count=34 seek=2 conv=notrunc

boot1:  descriptor_memory ${OBJS_BOOT1}
	${LD} -T ${LDSCRIPT_DIR}/floppy1.ldscript -o $@.elf  ${OBJS_BOOT1}
	${OBJCOPY} -I elf32-i386 -O binary $@.elf $@

boot2:  ${OBJS_BOOT2}
	${LD} -T ${LDSCRIPT_DIR}/floppy2.ldscript -o boot2.elf  ${OBJS_BOOT2} ${LIBS} -e start
	${OBJCOPY} -I elf32-i386 -O binary $@.elf $@

clean::
	-rm -f ${OBJS_BOOT1} ${OBJS_BOOT2} ${PROGS} *.elf *.core descriptor_memory gdt.h gdt.s ${FLOPPYIMAGE}

distclean: clean
	-rm -f disk.img

check1:
	${OBJDUMP} -x boot1.elf
	${OBJDUMP}  -D -m i8086 boot1.elf
	${OBJDUMP} -b binary -m i8086 -D boot1

check2:
	${OBJDUMP} -b binary -m i8086 -D boot2
#	${OBJDUMP} -x boot2.elf
#	${OBJDUMP} -D  boot2.elf

write_disk:
	dd if=${FLOPPYIMAGE} of=/dev/rfd0a bs=18k count=80

qemu:
	qemu  -serial stdio -std-vga -fda ./floppy.img
