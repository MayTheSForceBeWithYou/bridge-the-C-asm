NAME := prog
ASM := start.s
AS ?= as
LD ?= ld
OBJDUMP ?= objdump

.PHONY: all bin disasm run clean

all: bin
bin: $(NAME)

$(NAME).o: $(ASM)
	$(AS) --64 -g -o $@ $<

# Default link without custom script so `make` always works;
# student switches to -T link.ld when ready.
$(NAME): $(NAME).o
	$(LD) -o $@ $<

.PHONY: with-script
with-script: $(NAME).o link.ld
	$(LD) -T link.ld -o $(NAME) $(NAME).o

disasm: $(NAME)
	$(OBJDUMP) -d $(NAME) > $(NAME).lst

run: $(NAME)
	./$(NAME)

clean:
	rm -f $(NAME) $(NAME).o $(NAME).lst
