subdir = minlib/h8300h/advanced minlib/h8300h/normal liblua/h8300h/advanced board

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
