H8WRITE_DIR = ${H8_DIR}/h8write
BIN_DIR = ${ROOT_DIR}/bin
PROG = ${notdir ${PWD}}.mot

romwrite: rom h8write
	${BIN_DIR}/h8write -${H8ARCH} ${PROG}

h8write:
	cc ${H8WRITE_DIR}/h8write.c -o ${BIN_DIR}/h8write

