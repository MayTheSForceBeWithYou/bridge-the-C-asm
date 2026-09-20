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
4. Optional: point a throwaway `main` at `my_check.h` and prove a failing
   check exits non-zero. Do not change `prog.c` for that experiment.

## Done when

- `./prog` prints a FAIL with file:line and exits 1 (shipped demo).
- `my_check.h` has working `CHECK` / `CHECK_EQ` / `test_report` (your work).
- You can explain one difference you saw in the diff without reading a
  textbook.

## Lookup

`man 3 printf`, `info cpp` (stringification).
