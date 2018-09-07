H8SXWRITE_DIR = ${CPU_DIR}/h8sxwrite
PROG = a.mot

romwrite: all h8sxwrite
	${H8SXWRITE_DIR}/h8sxwrite -f 12 -d 0 -m 0 ${PROG}

h8sxwrite:
	${MAKE} all -C ${H8SXWRITE_DIR}
