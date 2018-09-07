subdir = minlib pcat_diskimage pcat_bootloader pcat_kernel pcat_floppyimage pcat_bootloader_installer

all:
	for i in ${subdir};				\
	do						\
		cd $$i && ${MAKE} all && cd ..;		\
	done

clean:
	for i in ${subdir};				\
	do						\
		cd $$i && ${MAKE} clean && cd ..;	\
	done

test:	all
	cd pcat_bootloader; ${MAKE} install_bootsector; cd ..
	cd pcat_kernel; ${MAKE} install_hdd; cd ..
	cd pcat_diskimage; ${MAKE} test
