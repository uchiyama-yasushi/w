KERN_CORE_OBJS	= assert.o
KERN_DELAY_OBJS = delay.o delay_subr.o
KERN_THREAD_OBJS = thread.o thread_stack.o thread_subr.o thread_machdep.o
KERN_FIBER_OBJS = fiber.o fiber_subr.o fiber_machdep.o
KERN_MONITOR_OBJS = monitor.o
KERN_TIMER_OBJS	= timer_dispatch.o timer.o
KERN_SHELL_OBJS	= shell.o prompt.o
KERN_CONSOLE_OBJS = console.o boot_console.o
KERN_BUFCONSOLE_OBJS = ${KERN_CONSOLE_OBJS} console_machdep.o	\
serial_console.o
KERN_RINGBUF_OBJS = ringbuffer.o rbuf_nolock.o