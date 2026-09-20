# 56 — opt levels timed (practice)

Read `LESSON.md` first. This file is only the lab.

## Time and diff

1. Finish `work()` (dependent loop); keep `rdtscp_now` working.
2. Build/run at `-O0`, `-O2`, and `-O3 -march=native`; tee outputs.
3. Save asm per level; diff / `objdump -d`; explain cycle ratios.

## Done when

- Three timings + notes linking instruction shape to speedup.

## Lookup

`man 1 gcc`, `man 1 objdump`, exercise 55.
