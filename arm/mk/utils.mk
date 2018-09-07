LPC23XXWRITE_DIR = ${CPU_DIR}/lpc23xxwrite
PROG = ${notdir ${PWD}}.bin

romwrite: all lpc23xxwrite
	${LPC23XXWRITE_DIR}/lpc23xxwrite -d ${SERIAL_DEVICE} -f 16000 -s 230400 ${PROG}

lpc23xxwrite:
	${MAKE} all -C ${LPC23XXWRITE_DIR}
