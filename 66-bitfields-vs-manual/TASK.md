# 66 — bitfields vs manual pack (Track G)

## Goal
Compare C bitfields vs shift/mask packing: `sizeof` and generated asm.

## Do
1. Define a bitfield struct and a manual `uint32_t` pack API for the same layout.
2. Print `sizeof`; implement get/set; `make asm` and compare.

## Success
Same logical values via both APIs; notes on sizeof + asm differences. `ok`.

## Refs
`man 1 gcc`, `man 1 objdump`, exercise 20
