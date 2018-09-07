#
#
ROOT_DIR	= ../..
include ../mk/dir.mk

include ${MK_DIR}/utils.mk

subdir += h8_monitor h8_overlay
subdir += aki dro01 dro02
subdir += ud01_monitor ud01_minkernel ud01_overlay	\
ud01_standalone_thread ud01_standalone_fiber ud01_minmonitor
subdir += tiny3664_monitor tiny3694_monitor tiny_test0 tiny_overlay	\
tiny_overlay_fiber tiny_standalone_thread
subdir += dro01_io
#
subdir += tiny_i2c_kxp84 ud01_mmc  dro01_io_test
subdir += ud01_overlay_fiber ud01_luamonitor ud01_motor	\
ud01_standalone ud01_lua ud01_minlua

all:
	for i in ${subdir};					\
	do							\
		cd $$i;${MAKE} all;[ $$? -eq 0 ] || exit 1;cd ..;\
	done

clean:
	for i in ${subdir};				\
	do						\
		cd $$i && ${MAKE} clean && cd ..;	\
	done

