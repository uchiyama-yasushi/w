subdir = test0 simple_monitor test_standalone test_thread test_mmc	\
test_i2c test_adc logger10 test_setjmp test_lua test_v7fs

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
