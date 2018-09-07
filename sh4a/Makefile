subdir =  minlib board

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
