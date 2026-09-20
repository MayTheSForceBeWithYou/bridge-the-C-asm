# 82 — Fake the syscall (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make && ./prog; echo $?`. Seven checks, exit 0.
2. Before writing anything, predict which checks would fail for a `greet`
   that calls `ops->write` and returns 0 unconditionally. Write the numbers
   down.
3. Write `student_greet.c` and build it against the unmodified `prog.c`:

   ```bash
   make clean && make SRCS="prog.c sys_ops.c student_greet.c"
   ```

   Then deliberately write the ignore-the-return-value version and check
   your prediction from step 2.
4. Handle the short write properly instead of failing on it: loop until all
   bytes are written or `write` returns `-1`. Add `sys_fake_short_next()` to
   a fresh scenario in `prog.c` and assert your loop reports success with
   three bytes total across two calls.
5. Swap in the real leaves (`.write = sys_real_write`) with `fd = 1` and run
   it. Confirm `hi` reaches your terminal. Note what you gave up: the byte
   counters are gone, and there is no way to make that write fail.

## Done when

- Your `student_greet.c` passes all seven checks.
- Your step-2 prediction matched the actual failures.
- You have a retry loop that survives a short write, proved by a check.
- You can say in one sentence what the fake replaces — the kernel entry
  point, not "I/O."

## Lookup

`man 2 write` (return value), lessons 78–81.
