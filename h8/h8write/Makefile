UNAME := ${shell uname}

all:	h8write

h8write:	h8write.c
	cc h8write.c -o h8write

clean:
ifneq (,$(findstring CYGWIN, $(UNAME)))
	rm -f h8write.exe
else
	rm -f h8write
endif