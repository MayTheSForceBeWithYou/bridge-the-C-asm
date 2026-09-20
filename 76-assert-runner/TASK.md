# 76 — Assert runner (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`. Confirm you see a FAIL line with `prog.c` and a
   line number, then `exit=1` from the shell.
2. Open `my_check.h`. Fill in the TODOs: counters, `CHECK`, `CHECK_EQ`, and
   `test_report` that returns non-zero on failure. Keep it small.
3. Diff your work against the harvested header:

   ```bash
   diff -u my_check.h ../test/check.h | less
   ```

   Expect differences: the harvested file also has `CHECK_STR` and
   `CHECK_PTR`. Your shape for `CHECK` / `CHECK_EQ` should be recognizable.
4. Prove your header independently. Write `my_prog.c` with a `main` that
   includes `my_check.h`, runs one passing and one failing check, and
   returns `test_report()`. Build and run it:

   ```bash
   gcc -Wall -Wextra -g -o my_prog my_prog.c && ./my_prog; echo $?
   ```

   You want a FAIL line naming `my_prog.c` and exit 1. Leave `prog.c` alone.
5. Make one check pass that should not: write `CHECK_EQ` so it compares with
   `=` instead of `==`, or forget to increment `tests_failed`. Confirm the
   runner now reports success on a broken claim. That is the failure mode a
   test framework has and your code does not.

## Done when

- `./prog` prints a FAIL with file:line and exits 1 (shipped demo).
- `my_check.h` has working `CHECK` / `CHECK_EQ` / `test_report` (your work).
- You can explain one difference you saw in the diff without reading a
  textbook.

## Lookup

`man 3 printf`, `info cpp` (stringification).
