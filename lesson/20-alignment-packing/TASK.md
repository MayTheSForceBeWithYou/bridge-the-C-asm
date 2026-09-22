# 20 — Packed vs aligned structs (practice)

Read `LESSON.md` first — especially **Decode loads for each twin**. This file is
only the lab.

## Build and inspect

1. Define the same logical layout twice: normal and `__attribute__((packed))`.
2. Write functions that read each field through a pointer. Print `sizeof`.
3. `make asm` and compare offsets / instruction shapes. Optionally
   `objdump -h` on the `.o`.

## Done when

- You showed how load offsets differ between natural and packed.
- You noted whether asm looks clumsier (splits, extra ops) or only denser /
  misaligned-but-single-`mov`.
- `sizeof` evidence matches your offset story.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info gcc`.
