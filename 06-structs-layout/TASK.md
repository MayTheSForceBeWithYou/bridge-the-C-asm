# 06 — Struct layout / padding (Track A)

## Goal
See padding and alignment as concrete offsets in asm.

## Do
1. Define at least two structs with intentional padding, e.g.
   `{ char; int; char; }` vs `{ int; char; char; }` vs a reordered form.
2. Write a function that takes a pointer to each and reads every field.
3. `make asm` and also print `sizeof` / guessed offsets from C.

## Observe / answer
- What load offsets appear for each field in the `.s`?
- How does that match `sizeof`? Where is the padding?
- Optional: compare with a `__attribute__((packed))` version (preview of 20).

## Refs
`man 1 gcc`, `man 1 objdump`, `info gcc`
