
CPUARCH		= x86_64netbsd
GNUARCH		= x86_64-netbsd-elf

include ${ROOT_DIR}/x86_64netbsd/mk/dir.mk
include ${MK_DIR}/rules.mk
include ${MK_DIR}/objs.mk
include ${ROOT_DIR}/filesystem/filesystem.mk