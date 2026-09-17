# 56 — opt levels timed (Track F)

## Goal
Time the **same** loop at `-O0`, `-O2`, `-O3 -march=native`; connect cycle
gaps to `objdump` shape (Track A method + clock).

## Do
1. Finish `work()` (simple dependent loop). Keep `rdtscp_now` working.
2. Build/run:
```
make clean && make O=0 && ./prog | tee o0.txt
make clean && make O=2 && ./prog | tee o2.txt
make clean && make CFLAGS='-ggdb -Wall -Wextra -O3 -march=native' && ./prog | tee o3.txt
make O=0 asm && cp prog.s prog.O0.s
make O=2 asm && cp prog.s prog.O2.s
```
3. Diff asm / `objdump -d` across levels; explain the cycle ratio.

## Success
Three timings + notes linking instruction shape to speedup.

## Refs
`man 1 gcc`, `man 1 objdump`, exercise 55
