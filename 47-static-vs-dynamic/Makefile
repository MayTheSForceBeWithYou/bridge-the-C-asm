NAME := hello
CLEAN_EXTRA = rm -f hello.dyn hello.static
include ../common.mk

.PHONY: static
static: hello.c
	$(CC) $(CFLAGS) -static -o hello.static hello.c || \
	$(CC) $(CFLAGS) -static-pie -o hello.static hello.c || \
	echo "NOTE: static link failed — analyze dynamic build deeply; see TASK.md"
