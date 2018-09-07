subdir = test0 test1 test2 test3 test4 test5

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
