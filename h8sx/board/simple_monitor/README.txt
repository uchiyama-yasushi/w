bitrate: 57600bps. see ../../1655/boot_console.c
command: l (load Motorola S-recode file and then execute)

ROM WRITE MODE: Short J1 on board.

also read ../../doc/startup.txt

For NetBSD environment.
chown user:staff /dev/dtyU*
chown user:staff /dev/ugen*

$ tip hmon57600
connected
SIO@
peripheral_power: 0xfffdcc (1000) ->1
stack_start: 0xffc000
RAM data: 0xff77c8-0xff7828 96byte
bss: 0xff7830-0xff93e8 7096byte
VBR: 0xff2000 (0xff2000)
MCU mode: 6
DTC: short-address mode
Internal RAM: enabled
External bus: enabled
Instruction fetch: 32bit
MAC instruction: non-saturation mode
Interrput Control mode: 2
PA7 clock output: On
Input clock: 12000000Hz, MDCLK=0
System clock: x4 (48000000Hz)
Peripheral clock: x2 (24000000Hz)
External clock: x4 (48000000Hz)
H8SX/1655 Simple Monitor Build Mar 24 2011 21:43:42
mon> ~
[EOT]
$ pwd
/home/uch/os/w/h8sx/board/test5.fiber
$ tip hmon57600
connected

mon> l
~>Local file name? a.mot
2814 lines transferred in 11 seconds 
!
Read D byte. 
Start address: 0xff3942
SIO@
peripheral_power: 0xfffdcc (1000) ->1
stack_start: 0xffc000
RAM data: 0xff77c8-0xff7828 96byte
bss: 0xff7830-0xff93e8 7096byte
VBR: 0xff2000 (0xff2000)
MCU mode: 6
DTC: short-address mode
Internal RAM: enabled
External bus: enabled
Instruction fetch: 32bit
MAC instruction: non-saturation mode
Interrput Control mode: 2
PA7 clock output: On
Input clock: 12000000Hz, MDCLK=0
System clock: x4 (48000000Hz)
Peripheral clock: x2 (24000000Hz)
External clock: x4 (48000000Hz)
thread_stack_setup: ff7d2c-ff7930 (400)
thread_stack_setup: ff812c-ff7d30 (400)
thread_stack_setup: ff852c-ff8130 (400)
thread_stack_setup: ff892c-ff8530 (400)
thread_create_no_stack: [2]:uart recv (no stack)
thread_start: [2]
thread_create_no_stack: [3]:uart send (no stack)
thread_start: [3]
sizeof long int: 4
sizeof long long: 8
sizeof long: 4
sizeof int: 4
sizeof short: 2
user> 
user> 



