
GCC_VERSION=4.6.0
#GCC_VERSION=4.3.2
CFLAGS += -D__WOS__
include ${ROOT_DIR}/mk/local_conf.mk
PATH = ${ROOT_DIR}/tools/bin:${ROOT_DIR}/bin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/pkg/bin