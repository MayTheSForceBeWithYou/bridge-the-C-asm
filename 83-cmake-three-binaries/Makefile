# Hand-maintained overlapping source lists — the shape this exercise is
# about, copied from the way ch24/ch32 grew. Read the three lists and find
# the discrepancy before you build anything.

CC ?= cc
CFLAGS ?= -ggdb -Wall -Wextra -O0

SOURCES      = main.c         combat.c        config.c rng.c
TEST_SOURCES = test_combat.c  combat_legacy.c config.c rng.c
FUZZ_SOURCES = fuzz_parsers.c combat.c        config.c rng.c

.PHONY: all skew clean

all: game run_tests fuzz_parsers

game: $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

run_tests: $(TEST_SOURCES)
	$(CC) $(CFLAGS) -o $@ $(TEST_SOURCES)

fuzz_parsers: $(FUZZ_SOURCES)
	$(CC) $(CFLAGS) -o $@ $(FUZZ_SOURCES)

# Run the two binaries side by side. Both build, both exit 0, and they
# disagree about what the game library computes.
skew: game run_tests
	@./game | grep combat_damage
	@./run_tests | grep combat_damage

clean:
	rm -f game run_tests fuzz_parsers
	rm -rf build
