# 83 — CMake three binaries (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Read the three source lists at the top of `Makefile` before building
   anything. Write down which module you think each binary links.
2. `make skew`. Explain the two numbers before you read any further.
3. `./run_tests; echo $?`. Note that the suite is green. Find the assertion
   that *should* have caught this and explain why it did not.
4. Add one check to `test_combat.c` that fails against `combat_legacy.c` and
   passes against `combat.c`. Rebuild. You now have a red suite caused by a
   build defect, not a logic defect — sit with that for a second.
5. Fix it in the build rather than the test: point `TEST_SOURCES` at
   `combat.c`. Confirm green, and confirm `make skew` now agrees.
6. Fill `student_CMakeLists.txt` yourself — one library, three executables,
   no shared `.c` named twice. Then:

   ```bash
   cp student_CMakeLists.txt /tmp/CMakeLists.txt   # keep the reference intact
   cmake -S . -B build -G Ninja && cmake --build build
   ./build/game && ./build/run_tests
   ```

7. Open `~/dev/c/LearnCwithRPG-terminal/code/ch24/Makefile` (read-only) and
   count how many `.c` files appear in more than one of the three lists.

## Done when

- You have seen `game` and `run_tests` disagree, with `run_tests` exiting 0.
- Your added check fails under the legacy module and passes under the real
  one — you chose an input where the two rules differ.
- Both the Make and CMake builds produce three binaries that agree.
- You can state in one sentence why "the tests pass" was not evidence.

## Lookup

`cmake-commands(7)`, your ch24 and ch32 Makefiles.
