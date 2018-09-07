#
# sysctl -w vfs.generic.usermount=1
# chown ${USER}:${GROUP} /mnt /dev/vnd0* /dev/rvnd0*
#

ROOT_DIR= ../..
include ../mk/pcat.mk

DISKSIZE_MB	= 16
PARTITION_PL	= ${ROOT_DIR}/bin/w_partition.pl
DOS_IMAGE	= freedos_floppy.img

#----------------------------------------------------------------------

all:	${DISKIMAGE} install_netbsd

clean::
	-rm -f ${DISKIMAGE} disk_format.sh

# 18sector 2head 80cylinder 512byte/sector
floppy:
	dd if=/dev/zero of=floppy.img bs=18k count=80

freedos_hdd:
	qemu -fda ${DOS_IMAGE} -hda ../freedos/freedos_all.img

install:	all

${DISKIMAGE}:
# 1st sector must be zero filled.
ifneq (,$(findstring NetBSD, $(UNAME)))
	dd if=/dev/zero of=${DISKIMAGE} bs=1m count=${DISKSIZE_MB}
else
	dd if=/dev/zero of=${DISKIMAGE} bs=1m count=${DISKSIZE_MB}
endif
	vnconfig vnd0 ${DISKIMAGE}
	${PARTITION_PL} > disk_format.sh
	sh disk_format.sh
	fdisk vnd0
	disklabel vnd0
	vnconfig -u vnd0
	-rm -f foo new

freedos:
	qemu  -serial stdio -vga std -fda ${DOS_IMAGE} -hda ${DISKIMAGE} -boot a

msdos:
	qemu  -serial stdio -vga std -fda ~/h8/msdos-6.2.img -hda ${DISKIMAGE} -boot a

# format c: /s

install_netbsd:
	vnconfig vnd0 ${DISKIMAGE}
	mount -o nodev,nosuid /dev/vnd0a /mnt
	chown -R ${USER}:${GROUP} /mnt
	rm -f /mnt/boot
	cp /usr/mdec/boot /mnt
	umount /mnt
	installboot -m i386 -v /dev/rvnd0a /usr/mdec/bootxx_ffsv2
# -o console=com0
	vnconfig -u vnd0

mount:
	vnconfig vnd0 ${DISKIMAGE}
	mount -o nodev,nosuid /dev/vnd0a /mnt
	chown -R ${USER}:${GROUP} /mnt

mount_sysvbfs:
	vnconfig vnd0 ${DISKIMAGE}
	mount_sysvbfs -o nodev,nosuid /dev/vnd0f /mnt
	chown -R ${USER}:${GROUP} /mnt

mount_msdos:
	vnconfig vnd0 ${DISKIMAGE}
	mount_msdos -o nodev,nosuid /dev/vnd0e /mnt
	chown -R ${USER}:${GROUP} /mnt

umount:
	umount /mnt
	vnconfig -u vnd0

test:
#	qemu -fda ${PROG}
#	qemu -nographic -serial stdio -hda ${PROG}
	qemu  -serial stdio -vga std -hda ${DISKIMAGE}
# C-a x : exit QEMU