# 64 — memcpy pun vs union (Track G)

## Goal
Same type pun via `memcpy` (standard-friendly) vs union; compare `-O2` asm.

## Do
1. Implement `bits_memcpy` and `bits_union`.
2. `make O=2 asm disasm` — do they collapse to the same load?
3. Note the "union punning vs memcpy" distinction in your notes.

## Success
Both return identical bits; `-O2` asm is essentially equivalent. `ok`.

## Refs
`man 3 memcpy`, `man 1 gcc`, `man 1 objdump`
