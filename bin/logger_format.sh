#!/bin/sh
USER=`id -u`
GROUP=`id -g`

./logger_partition.pl > aaa
sh aaa
[ $? -eq 0 ] || exit 1
mount_sysvbfs -o nodev,nosuid /dev/ld0a /mnt
[ $? -eq 0 ] || exit 1
chown -R $UESR:$GROUP /mnt
ls -al /mnt
umount /mnt
