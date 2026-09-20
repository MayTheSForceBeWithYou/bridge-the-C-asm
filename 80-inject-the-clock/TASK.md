# 80 — Inject the clock (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make && ./prog; echo $?`. Confirm six checks pass and exit 0.
2. Write `student_clock.c` — your own `stamp_event` and `cooldown_ready` —
   and build it against the same unmodified `prog.c`:

   ```bash
   make clean && make SRCS="prog.c clock.c student_clock.c"
   ```

   Green means your seam behaves like the reference.
3. Prove the test can detect a cheat. Temporarily make your `cooldown_ready`
   call `time(NULL)` directly instead of `clock->now()`. Rebuild, watch which
   checks fail, and explain why *those* checks and not the others. Revert.
4. Add a check of your own: a cooldown that started exactly `seconds` ago.
   Decide whether ready-at-exactly-elapsed is correct, then assert your
   decision.
5. Open `~/dev/c/LearnCwithRPG-terminal/code/ch24/game.c` around line 1122
   (read-only). Confirm the seed is a single ambient call at the composition
   root, then grep the rest of that tree for other `time(` calls and judge
   each one.

## Done when

- Your `student_clock.c` passes all six checks against the shipped `prog.c`.
- You have seen the suite go red when the code bypasses the injected clock,
  and you can name which check caught it.
- You can point at one production seed site and say why logic elsewhere must
  not call `time` itself.

## Lookup

`man 2 time`, lesson 78.
