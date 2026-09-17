# Shared rules for c-and-asm-grind exercises.
# Include from an exercise Makefile after setting NAME (and optionally SRCS).
# Optional per-exercise: LDLIBS += -lpthread
# Optional: LDFLAGS += -static / -no-pie / -nostdlib as noted in TASK.md.

CFLAGS ?= -ggdb -Wall -Wextra -O0
ifdef O
  CFLAGS := $(filter-out -O0 -O1 -O2 -O3 -Os -Og,$(CFLAGS)) -O$(O)
endif

CC ?= gcc
AS ?= as
LD ?= ld
OBJDUMP ?= objdump
LDLIBS ?=

SRCS ?= $(NAME).c
OBJS ?= $(SRCS:.c=.o)
OBJS := $(OBJS:.s=.o)

.PHONY: all bin preprocess asm obj disasm disasm-intel run clean

all: bin

bin: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) -c -o $@ $<

PRIMARY_C := $(firstword $(filter %.c,$(SRCS)))

preprocess: $(PRIMARY_C)
	$(CC) $(CFLAGS) -E -o $(NAME).i $(PRIMARY_C)

asm: $(PRIMARY_C)
	$(CC) $(CFLAGS) -S -fverbose-asm -o $(NAME).s $(PRIMARY_C)

obj: $(OBJS)

disasm: $(NAME)
	$(OBJDUMP) -d $(NAME) > $(NAME).lst

disasm-intel: $(NAME)
	$(OBJDUMP) -d -M intel $(NAME) > $(NAME).lst.intel

run: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) $(OBJS) $(NAME).i $(NAME).s $(NAME).lst $(NAME).lst.intel \
	      *.o *.i *.lst *.lst.intel a.out
	$(CLEAN_EXTRA)
