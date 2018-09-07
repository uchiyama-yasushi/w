#TOOLDIR		= ${shell pwd -L}
# GNU pwd don't have -L option
TOOLDIR	= ${shell pwd}
TARGETS=arm-elf i386-elf h8300-elf sh-elf rx-elf
#TARGETS="x86_64-netbsd-elf"

all:
	mkdir work; [ $$? -eq 0 ] || exit 1;			\
	cd work;						\
	for i in ../dist/*.tar.*;				\
	do							\
		tar zxf $$i;					\
	done;							\
	TARGETS="${TARGETS}" MAKE=${MAKE} ../../bin/build_env.sh ${TOOLDIR}

build:
	cd work && TARGETS="${TARGETS}" MAKE=${MAKE} ../../bin/build_env.sh ${TOOLDIR}

clean:
	rm -rf work

distclean: clean
	rm -rf ${TARGETS} bin include info lib libexec man share


