# 81 — Inject file I/O (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make && ./prog; echo $?`. Three checks, exit 0.
2. Write `student_parse.c` against `file_ops.h` and build it in place of the
   reference:

   ```bash
   make clean && make SRCS="prog.c file_ops.c student_parse.c"
   ```

3. Break the ownership contract on purpose: have your `read_all` fake return
   a pointer to a string literal instead of a `strdup`. Run it. Then run it
   under ASan and compare what each tells you:

   ```bash
   gcc -fsanitize=address -g -o prog.asan prog.c file_ops.c student_parse.c && ./prog.asan
   ```

4. Add checks for two rules the current suite does not pin down: a line that
   is only whitespace, and a line whose `=` is the first character. Decide
   what each should do before you write the assertion.
5. Open `~/dev/c/LearnCwithRPG-terminal/code/ch24/config.c` (read-only). Find
   the `fopen`. Sketch — no need to build it — what `config_load` would look
   like split into a reader and a parser.

## Done when

- Your `student_parse.c` passes the shipped checks.
- You have seen what breaking the free contract does, with and without ASan.
- Your two added checks encode a decision you can defend, not a guess about
  what the reference happens to do.

## Lookup

`man 3 strtok_r`, `man 3 strdup`, lesson 80.
