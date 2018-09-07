#!/bin/sh
gmake distclean
gmake kickstart 1> kickstart.log 2>&1
gmake all 1> all.log 2>&1
