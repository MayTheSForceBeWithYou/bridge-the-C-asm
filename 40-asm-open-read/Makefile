# Freestanding: as + ld, no libc
NAME := cat_task
ASM := cat_task.s
AS ?= as
LD ?= ld
OBJDUMP ?= objdump
.PHONY: all bin obj disasm disasm-intel run clean
all: bin
bin: $(NAME)
$(NAME).o: $(ASM)
	$(AS) --64 -g -o $@ $<
$(NAME): $(NAME).o
	$(LD) -o $@ $<
obj: $(NAME).o
disasm: $(NAME)
	$(OBJDUMP) -d $(NAME) > $(NAME).lst
disasm-intel: $(NAME)
	$(OBJDUMP) -d -M intel $(NAME) > $(NAME).lst.intel
run: $(NAME)
	./$(NAME); echo "exit=$$?"
clean:
	rm -f $(NAME) $(NAME).o $(NAME).lst $(NAME).lst.intel
