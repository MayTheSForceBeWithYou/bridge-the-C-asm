# 80 — Inject the clock (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`; confirm exit 0.
2. Open `ch24/game.c` around line 1122 in the RPG repo (read-only). Note
   that the seed is a single ambient call — the composition root.
3. Fill `student_clock.c` if you want the reps; keep the linked `clock.c`
   as the reference.

## Done when

- Frozen stamps match across two calls.
- You can point at one production seed site and say why logic elsewhere
  should not call `time` itself.

## Lookup

`man 2 time`, lesson 78.
