#
#
#ROOT_DIR	=${shell pwd -L}
# GNU pwd don't have -L option
ROOT_DIR	=${shell pwd}
LOCAL_CONF	=${ROOT_DIR}/mk/local_conf.mk
USER		=${shell id -u}
GROUP		=${shell id -g}
TOOLDIR		=${ROOT_DIR}/tools
TARGETS	= h8 x86 arm sh4a sh2a h8sx

all:
	for i in ${TARGETS};		\
	do				\
		${MAKE} all -C $$i;	\
	done;

kickstart:	gen_localconf
	${MAKE}	all -C tools

gen_localconf:
	echo "TOOLDIR=${TOOLDIR}" > ${LOCAL_CONF}
	echo "USER=${USER}" >> ${LOCAL_CONF}
	echo "GROUP=${GROUP}" >> ${LOCAL_CONF}

clean:
	for i in ${TARGETS};		\
	do				\
		${MAKE} clean -C $$i;	\
	done;
	rm -f bin/h8write bin/srecdump TAGS

distclean: gen_localconf clean
	rm -f ${LOCAL_CONF}
	${MAKE} distclean -C tools

tags:
	find `ls ${ROOT_DIR}| grep -v tools` \( -name "*.[chS]" -or -name "Makefile" -or -name "*.mk" \)  -print |xargs etags

tar:
	tar cfz w`date +%y%m%d-%H`.tar.gz `ls ${ROOT_DIR}| grep -v tools`

gomi:
	find `ls ${ROOT_DIR}| grep -v tools` \( -name "*.[oa]" -or -name "*.core" -or -name "*~" -or -name ".deps" \)  -print|xargs echo


srecdump:
	g++ ${ROOT_DIR}/kernel/srec.c -I${ROOT_DIR}/include -DSTANDALONE -o ${ROOT_DIR}/bin/srecdump

include ${ROOT_DIR}/h8/mk/utils.mk
