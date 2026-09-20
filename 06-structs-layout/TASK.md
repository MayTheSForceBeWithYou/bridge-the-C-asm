# 06 — Struct layout / padding (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Define at least two structs with intentional padding differences (reorder
   fields). Optionally add a packed twin as a preview of lesson 20.
2. Write functions that take a pointer to each and read every field.
3. `make asm`; also print `sizeof` / guessed offsets from C.
4. Match load offsets in `.s` to your layout notes.

## Done when

- You explained where padding sits in each layout.
- Asm offsets and `sizeof` agree with your explanation.
- You noted how reordering (or packing) changed the picture.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info gcc`.
