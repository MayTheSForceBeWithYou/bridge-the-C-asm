# 20 — Packed vs aligned structs (Track A)

## Goal
See packing change offsets — and whether asm shows clumsier accesses.

## Do
1. Define the same logical layout twice: normal and
   `__attribute__((packed))`.
2. Functions that read each field through a pointer.
3. `make asm` and compare offsets / instruction sizes. Print `sizeof`.

## Observe / answer
- How do load instructions differ (size, offset, split loads)?
- Is a "cost" obvious in the asm, or only plausible from instruction count?
- Confirm section/align notes with `objdump -h` on the `.o` if useful.

## Refs
`man 1 gcc`, `man 1 objdump`, `info gcc`
