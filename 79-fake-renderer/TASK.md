# 79 — Fake renderer (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make && ./prog; echo $?`. Ten checks, exit 0.
2. Read the wall-bump checks in `prog.c`. Say which lines encode "the player
   was drawn at the old position" rather than merely "a draw happened."
3. Fill the TODOs in `render_fake.c` — the recording and the reset — and
   build it in place of the reference:

   ```bash
   make clean && make SRCS="prog.c player.c render_fake.c"
   ```

4. Delete your `render_fake_reset` body so it does nothing, and rebuild.
   Watch the second scenario fail while the first still passes, and explain
   why that ordering is what you would expect.
5. Weaken the suite on purpose: replace the two coordinate checks with
   `CHECK_EQ(fake.draw_count, 1)` alone. Then change `player.c` so a blocked
   move draws the player *past* the wall. Confirm the weakened suite stays
   green. Restore both.

## Done when

- Your `render_fake.c` passes all ten checks.
- You have watched a call-count-only suite miss a real bug that the
  coordinate checks catch.
- You can name the two fake failure modes from the lesson without looking.

## Lookup

`render_fake.h`, `../test/check.h`, lesson 78.
