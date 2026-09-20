# Hand-maintained overlapping lists — the problem this exercise names.
# Forgetting lib.c in one list silently tests the wrong thing.

CC ?= cc
CFLAGS ?= -ggdb -Wall -Wextra -O0

SOURCES = main.c lib.c
TEST_SOURCES = test_combat.c lib.c
FUZZ_SOURCES = fuzz_parsers.c lib.c

.PHONY: all game run_tests fuzz_parsers clean

all: game run_tests fuzz_parsers

game: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

run_tests: $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $@ $(TEST_SOURCES)

fuzz_parsers: $(FUZZ_SOURCES)
	$(CC) $(CFLAGS) -o $@ $(FUZZ_SOURCES)

clean:
	rm -f game run_tests fuzz_parsers
