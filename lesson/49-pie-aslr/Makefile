NAME := prog
CLEAN_EXTRA = rm -f prog.nopie
include ../../common.mk

.PHONY: nopie
nopie: prog.c
	$(CC) $(CFLAGS) -no-pie -o prog.nopie prog.c
