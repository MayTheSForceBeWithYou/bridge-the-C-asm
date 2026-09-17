# 58 — branch mispredict sorted vs shuffled (Track F)

## Goal
Sum values over a threshold on sorted vs shuffled data; measure with
`rdtscp`. Tie to exercise 31 (`cmov` vs real branches).

## Do
1. Fill array with random 0..255; copy; sort one copy.
2. Time `sum_thresh` on sorted vs shuffled (same content, same N).
3. Explain the gap via branch predictability / cmov (ex 31).

## Success
Printed cycle counts for both; sorted usually faster. `ok` on checksum.

## Refs
exercise 31, exercise 55, `man 1 gcc`
