NAME := write_test
SRCS := harness.c my_write.s
CLEAN_EXTRA = rm -f write_n write_n.o
include ../common.mk

AS ?= as
LD ?= ld

.PHONY: freestanding freestanding-run

freestanding: write_n

write_n.o: write_n.s
	$(AS) --64 -g -o $@ $<

write_n: write_n.o
	$(LD) -o $@ $<

freestanding-run: write_n
	./write_n; echo exit=$$?
