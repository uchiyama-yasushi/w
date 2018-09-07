ROOT_DIR= ../..

include ../mk/pcat.mk

OBJS_BOOT1 = boot1.o
OBJS_BOOT2 = boot2.o bios_console_vga.o bios_console_serial.o		\
bios_hdd.o gdt.o mode16.o mode32.o a20.o pckbc.o start.o console.o	\
bios_console.o shell.o prompt.o shell_command.o cpu.o assert.o nmi.o	\
bios_int13_rw_ext.o bfs.o bfs_init.o bootloader.o mbr.o disklabel.o	\
sysvbfs.o keyboard_timer.o load_elf.o bios_service.o pcat_mbr.o		\
bios_timer_subr.o bios_delay.o bios_delay_subr.o

#
#
PROGS	= boot1 boot2
DESCRIPTOR_FLAGS = -DBOOTSTRAP_GDT
FLAGS = -DBOOTSTRAP_GDT -DSTATIC=static -DDEBUG -DTHREAD_DISABLE	\
-DBOOTSTRAP_IDT -DPROMPT="\"boot> \""
MINLIB	= ${CPU_DIR}/minlib/minlib.a
${MINLIB}:
	${MAKE} -C ${CPU_DIR}/minlib
LIBS	= ${MINLIB}


# -DBIOSIO_DEBUG
#----------------------------------------------------------------------
all:	${PROGS}


boot1:  ${OBJS_BOOT1}
	${LD} -T ${LDSCRIPT_DIR}/harddisk1.ldscript -o $@.elf  ${OBJS_BOOT1}
	${OBJCOPY} -I elf32-i386 -O binary $@.elf $@

boot2:  descriptor_memory ${OBJS_BOOT2}
	${LD} -T ${LDSCRIPT_DIR}/harddisk2.ldscript -o boot2.elf  ${OBJS_BOOT2} ${LIBS} -e bootloader_saved
	${OBJCOPY} -I elf32-i386 -O binary $@.elf $@

clean::
	-rm -f ${OBJS_BOOT1} ${OBJS_BOOT2} ${PROGS} *.elf *.core descriptor_memory gdt.h gdt.s

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

install: install_hdd install_floppy install_bootsector

install_bootsector: ${PROGS}
# seek: output
# skip: input
#	dd if=boot1 of=${DISKIMAGE} bs=1 count=446 conv=notrunc
	dd if=boot1 of=${DISKIMAGE} bs=1 count=11 conv=notrunc
	dd if=boot1 of=${DISKIMAGE} bs=1 count=156 skip=62 seek=62 conv=notrunc
	dd if=boot1 of=${DISKIMAGE} bs=1 count=2 skip=510 seek=510 conv=notrunc
	dd if=boot2 of=${DISKIMAGE} bs=512 count=48 seek=1 conv=notrunc

install_hdd: ${PROGS}
	vnconfig vnd0 ${DISKIMAGE}
	mount_sysvbfs -o nodev,nosuid /dev/vnd0f /mnt
	chown -R ${USER}:${GROUP} /mnt
	cp ${PROGS} /mnt
	umount /mnt
	vnconfig -u vnd0

install_floppy: ${PROGS}
	vnconfig vnd0 ${FLOPPYIMAGE}
	mount_sysvbfs -o nodev,nosuid /dev/vnd0a /mnt
	chown -R ${USER}:${GROUP} /mnt
	cp ${PROGS} /mnt
	umount /mnt
	vnconfig -u vnd0


# qemu  -serial stdio -std-vga -fda ../fdboot.img -hda disk.img -cdrom ../fdfullcd.iso -boot a

