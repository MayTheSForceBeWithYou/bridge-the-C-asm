NAME := riprel
SRCS := riprel.s
CLEAN_EXTRA = rm -f riprel.abs riprel.abs.o riprel.nopie
include ../common.mk

.PHONY: absolute absolute-nopie nopie

# Same source, assembled with the absolute-addressing branch selected.
riprel.abs.o: riprel.s
	$(CC) -Wa,--defsym=USE_ABS=1 -c -o $@ $<

# Expected to FAIL at link time: absolute 32-bit relocation into a PIE.
absolute: riprel.abs.o
	$(CC) -o riprel.abs riprel.abs.o

# Same object, non-PIE link: now the relocation is representable.
absolute-nopie: riprel.abs.o
	$(CC) -no-pie -o riprel.abs riprel.abs.o

nopie: $(NAME).o
	$(CC) $(CFLAGS) -no-pie -o $(NAME).nopie $(NAME).o
