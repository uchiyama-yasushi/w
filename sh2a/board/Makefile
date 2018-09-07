subdir = test1_led test2_sci6 test3_sci6_putc_getc simple_monitor	\
monitor_loaded_program_sample test4_interrupt test5_cache test7_fiber	\
test8_setjmp test6_lua test6_1_minlua test9_misc

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
