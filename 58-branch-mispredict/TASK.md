# 58 — branch mispredict sorted vs shuffled (practice)

Read `LESSON.md` first. This file is only the lab.

## Measure

1. Fill array with random 0..255; copy; sort one copy.
2. Time `sum_thresh` on sorted vs shuffled (same content, same N) with `rdtscp`.
3. Explain the gap (predictability / cmov). Print `ok` on checksum.

## Done when

- Both cycle counts printed; sorted usually faster; checksum `ok`.

## Lookup

exercise 31, exercise 55, `man 1 gcc`.
