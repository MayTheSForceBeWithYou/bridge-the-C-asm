NAME := prog
ASM := start.s
AS ?= as
LD ?= ld
OBJDUMP ?= objdump

.PHONY: all bin explicit disasm run clean

all: bin
bin: $(NAME)

$(NAME).o: $(ASM)
	$(AS) --64 -g -o $@ $<

$(NAME): $(NAME).o
	$(LD) -o $@ $<

# Student: adjust this explicit recipe and explain flags in notes
explicit: $(NAME).o
	$(LD) -o $(NAME) $(NAME).o

disasm: $(NAME)
	$(OBJDUMP) -d $(NAME) > $(NAME).lst

run: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) $(NAME).o $(NAME).lst
