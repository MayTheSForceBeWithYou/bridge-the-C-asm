# 59 — cache stride (Track F)

## Goal
Sequential vs 64-byte-stride traversal over the same volume — L1 line size
on this machine is **64** (`getconf LEVEL1_DCACHE_LINESIZE`).

## Do
1. Allocate a large `char`/`int` buffer (tens of MiB).
2. Time sequential touch vs stride-64 touch (same number of accesses).
3. Explain using the 64-byte line fact (stated — no need to rediscover).

## Success
Two cycle counts; stride slower. Prints `ok`.

## Refs
exercise 55, `man 1 gcc`
