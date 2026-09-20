# 83 — Three binaries, one library graph
<!-- concepts: c.p4.build.cmake, c.p4.build.targets -->

Your RPG keeps three overlapping source lists: `SOURCES`, `TEST_SOURCES`,
and `FUZZ_SOURCES`. Each is maintained by hand, and nothing checks that they
agree about the shared half of the program. After this lesson you can say
exactly how that arrangement produces a green test suite that proves nothing,
and you can redraw the same program as one library target plus three
executables.

## What this lesson asks of you

Build this directory with its Makefile and run `make skew`. Two binaries
that both claim to use "the game library" will print different answers for
the same inputs, and the test suite will exit 0 while it happens. Then build
the same sources with the shipped `CMakeLists.txt` and watch the
disagreement disappear. Fill `student_CMakeLists.txt` from memory.

This is content *about* CMake. Exercises 01–82 still build with plain
`make`; nothing here changes that.

## Two failure modes, and only one of them is loud

Drop a shared module from one list and the usual result is an undefined
reference. The linker names the missing symbol, you add the file, you move
on. That failure costs a minute and teaches nothing, because it cannot
escape your terminal.

The expensive failure is the one that links. It needs a second definition of
the same symbol somewhere in the tree — an older module kept "just in case,"
a `_v2` file, a copy someone made while refactoring. Name that file in one
list and the shipped file in another, and every binary builds, every binary
runs, and two of them implement different rules. No warning fires, because
nothing is wrong at the level the compiler and linker inspect: each binary
got exactly one definition of `combat_damage`, which is all either tool asks.

That is the case this directory reproduces.

## The skew, in four lines of Makefile

```make
SOURCES      = main.c         combat.c        config.c rng.c
TEST_SOURCES = test_combat.c  combat_legacy.c config.c rng.c
FUZZ_SOURCES = fuzz_parsers.c combat.c        config.c rng.c
```

`combat.c` floors damage at zero. `combat_legacy.c` is the rule from before
the floor was added and returns negative numbers. The test list names the
legacy module. Nothing else in the three lists hints at it, and the columns
line up so neatly that the eye slides right over the one word that differs.

Now look at what the suite asserts. `combat_damage(10, 3) == 7` and
`combat_damage(5, 5) == 0` are both true under *either* rule — the floor
only changes behavior when `def` exceeds `atk`, and no check goes there. The
suite is green, honestly green, and entirely uninformative about the module
that ships.

## Worked example

```text
$ make skew
game:      combat_damage(3, 5) = 0
run_tests: combat_damage(3, 5) = -2
```

Same call, same header, two answers, both binaries built by the same `make`
invocation seconds apart. `./run_tests` exits 0 while printing `-2`.

Build the same sources through the target graph instead:

```bash
cmake -S . -B build -G Ninja && cmake --build build
./build/game && ./build/run_tests
```

Both report `0`. Nothing was fixed in any `.c` file. What changed is that
`combat.c` is now named once, inside `add_library(gamelib STATIC ...)`, and
the three executables depend on that node rather than on three transcriptions
of it. `combat_legacy.c` is named nowhere, which is the only honest thing to
do with a file you are not shipping.

It is tempting to read this exercise as "learn CMake syntax." That is wrong,
because the syntax here is four commands and you could get the same guarantee
from a carefully factored Makefile with one `OBJECTS` variable. The claim is
narrower and harder: **shared code should have one name in the build, and any
arrangement that requires you to keep two lists in agreement will eventually
fail to.** CMake is one way to stop maintaining the second list. Copying
`combat.c` into three `add_executable` source lines would reproduce the bug
in a new generator, and the tool would not stop you.

## Distinctions worth keeping straight

- **Target graph vs source list.** Edges you declare once, versus text you
  keep in sync by hand.
- **Missing symbol vs wrong symbol.** The first fails the build; the second
  passes the build, passes the tests, and ships.
- **A green suite vs a suite that links what you ship.** Only the second is
  evidence.
- **CMake as content vs CMake as this repo's build.** Taught here; not
  adopted for the other exercises.

## Check yourself

1. Why does dropping `combat.c` from a list entirely cost you less than
   naming `combat_legacy.c` in its place?
2. The suite asserts three things and passes under both rules. What input
   would have caught the skew, and why was it not in the list?
3. How many times should a shared `.c` be named in a healthy build?
4. What does `add_library` buy you that three `add_executable` lists do not?
5. Your own `TEST_SOURCES` omits every renderer (exercise 79). Is that the
   same defect as this one, or a different judgement?

## Key takeaways

- Hand-maintained overlapping lists are a defect waiting for a second
  definition to exist.
- The dangerous link error is the one that succeeds.
- Assertions that hold under both implementations cannot detect which one
  you linked; pick inputs where the rules disagree.
- One library node makes "which module ships" unaskable rather than merely
  answered correctly today.
- The graph is the lesson; CMake is the vehicle.

## Lookup (not the lesson)

- `cmake --help`, `cmake-commands(7)` — `add_library`, `add_executable`,
  `target_link_libraries`
- `~/dev/c/LearnCwithRPG-terminal/code/ch24/Makefile` — `SOURCES`,
  `TEST_SOURCES`, `FUZZ_SOURCES`
- `~/dev/c/LearnCwithRPG-graphics/code/ch32/Makefile` — the same three lists,
  grown larger

Now open `TASK.md` and do the practice.
