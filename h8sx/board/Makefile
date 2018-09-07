subdir = simple_monitor test1.LED.asm test2.LED.C test3.SCI4	\
test4.thread dro3 test5.fiber test6.lua test7.minlua

all:
	for i in ${subdir};					\
	do							\
		cd $$i;${MAKE} all;[ $$? -eq 0 ] || exit 1;cd ..;\
	done

clean:
	for i in ${subdir};				\
	do						\
		(cd $$i;${MAKE} clean);			\
	done
