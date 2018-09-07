subdir = minlib h8sxwrite liblua board

all:
	for i in ${subdir};					\
	do							\
		(cd $$i;${MAKE} all;[ $$? -eq 0 ] || exit 1);\
	done

clean:
	for i in ${subdir};				\
	do						\
		(cd $$i;${MAKE} clean);			\
	done
