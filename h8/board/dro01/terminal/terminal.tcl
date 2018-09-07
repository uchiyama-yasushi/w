#!/usr/pkg/bin/wish


if { $tcl_platform(os) == "NetBSD" } {
# specify dial-out device.
	if { $tcl_platform(machine) == "sh3el" } {
# SH3 SCIF
	    set device "/dev/dscif0"
	} else {
# NS16550
	    set device "/dev/dty00"
	}
    } else {
# Windows
	set device "COM2"
    }

set fd [open $device RDWR]
fconfigure $fd -mode 115200,n,8,1 -blocking 0
#fconfigure $fd -buffering line -blocking 1  -timeout 1000  -handshake rtscts

set row 0
set axis_name(0) X
set axis_name(1) Y
set axis_name(2) Z

foreach i { 0 1 2 } {
    set dro_offset($i) 0.
    set dro_sign($i) 1
    set dro_value($i) 0.
    set dro_power($i) 0
    set dro_speed($i) 0
    set dro_direction($i) 0
#    checkbutton .c$i -text $i: -variable dro_power($i) -command "dro_power $i" 	-font { {Helvetica} 50 bold} -borderwidth 3
#    .c$i configure -anchor e
    button .b_hardreset$i -text 電源 -height 1  -font { {Helvetica} 10 bold } -borderwidth 3 -command "dro_power $i"
    label .l$i -text $axis_name($i): -font { {Lucida} 40 bold} -borderwidth 3
    .l$i configure -anchor e

    checkbutton .cs$i -text 高速更新 -variable dro_speed($i) -command "dro_speed $i" -font { {Helvetica} 10 bold} -borderwidth 3 -height 4
    checkbutton .cd$i -text 方向 -variable dro_direction($i) -command "dro_direction $i" -font { {Helvetica} 10 bold} -borderwidth 3 -height 4

    .cs$i configure -anchor e
    .cd$i configure -anchor e

    button .b_softreset$i  -height 1 -width 7 -pady 0 -font {{Lucida} 40 bold} -command "dro_zero $i"

#   grid configure .c$i -column 0 -row $row
    grid configure .b_hardreset$i -column 0 -row $row
    grid configure .l$i -column 1 -row $row
    grid configure .b_softreset$i -column 2 -row $row
    grid configure .cs$i -column 3 -row $row
    grid configure .cd$i -column 4 -row $row
    incr row
}

button .e -text "終了" -command {
    set a [tk_messageBox -type yesno -default no -icon question -message "終了しますよ?"]
    if { $a == "yes" } { exit }
}
grid configure .e -row $row

proc dro_power { axis } {
    global fd
#    puts -nonewline $fd "ledtest 0 1"
    puts $fd [format "caliper_power %d" $axis];
    if {[catch {flush $fd}]} {
	puts "I/O error $fd"
    }
}

proc dro_speed { axis } {
    global fd
    puts $fd [format "caliper_fast %d" $axis];
    if {[catch {flush $fd}]} {
	puts "I/O error $fd"
    }
}

proc dro_direction { axis } {
    global fd
    puts $fd [format "dir %d" $axis];
    if {[catch {flush $fd}]} {
	puts "I/O error $fd"
    }
}

proc dro_zero { axis } {
    global fd
    puts $fd [format "zero %d" $axis];
    if {[catch {flush $fd}]} {
	puts "I/O error $fd"
    }
}

set val ""
    set axis 0
    set v 0

while { 1 } {
    update
    gets $fd buf
    if { [string length $buf] == 0 } {
	continue
    }
    set val $val$buf
    if { [string first  "]" $buf ] == -1 } {
	continue
    }

    scan $val "%d%d" axis v
    set v [expr $v / 100.0]
    set t [format "%5.2f" $v]
    .b_softreset$axis configure -text $t -anchor e
    set val ""
}