# 83 — CMake three binaries (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` — run `./game`, `./run_tests`, `./fuzz_parsers`.
2. Configure the shipped CMake file:

   ```bash
   cmake -S . -B build -G Ninja && cmake --build build
   ./build/run_tests
   ```

3. Fill `student_CMakeLists.txt` with one library and three executables.
   Do not paste `lib.c` into each executable's source list.
4. Open `ch24/Makefile` (read-only) and count how many times a shared
   `.c` appears across the three lists.

## Done when

- Both Make and CMake builds produce three binaries that run.
- You can explain the silent-skew failure mode in one sentence.

## Lookup

`cmake-commands(7)`, your ch24 Makefile.
