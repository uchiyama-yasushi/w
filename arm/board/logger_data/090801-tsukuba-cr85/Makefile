SHELL	= /bin/sh

CC = cc
LD = cc
PROG = logger_format
OBJS = check.o

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

data:	all preprocess
	./${PROG} log1.dat > log1.dat.all 2>&1
	./${PROG} log5.dat > log5.dat.all 2>&1

all:	${PROG}

clean:
	rm -f ${OBJS} ${PROG} *.lap *.speed *.all *.label \
	plap_kesshou_data_convert extract_speed_data *.raw_speed plap_cooked.dat


preprocess:
	${CC} -o plap_kesshou_data_convert plap_kesshou_data_convert.c
	${CC} -o extract_speed_data extract_speed_data.c
	./plap_kesshou_data_convert
	./extract_speed_data log4.dat
	cat log4.dat.raw_speed plap_cooked.dat > log5.dat

${PROG}:	${OBJS}
	${LD} -o ${PROG}  ${OBJS}
