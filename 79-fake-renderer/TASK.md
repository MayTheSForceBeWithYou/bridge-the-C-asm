# 79 — Fake renderer (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`; confirm exit 0 and zero failures.
2. Read the wall-bump checks in `prog.c`. Say out loud which lines encode
   "old position," not merely "a draw happened."
3. Implement `render_fake.c` (TODOs). When ready:

   ```make
   # in Makefile, switch the fake line:
   SRCS := prog.c player.c render_fake.c
   ```

   Rebuild and confirm exit 0 with *your* fake.
4. Break one assertion on purpose (wrong expected x). Confirm exit 1 and a
   `prog.c:N` FAIL line. Restore it.

## Done when

- Reference build exits 0.
- `render_fake.c` still contains `TODO` until you finish it; after you
  finish, your fake passes the same checks.
- You can name the two fake failure modes from the lesson without looking.

## Lookup

`render_fake.h`, `../test/check.h`, lesson 78.
