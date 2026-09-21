# 48 — PLT lazy binding (practice)

Read `LESSON.md` first. This file is only the lab.

## Inspect

1. Hello that calls `printf`; build dynamic (`make`).
2. `objdump -d -j .plt ./hello`, full disassembly, `readelf -r`, `readelf -d`.
3. Locate `printf@plt`; note what it jumps through. Optional gdb: first vs later call.
4. Write a short lazy-binding explanation in your notes.

## Done when

- You named the PLT stub address and described lazy binding from what you saw.

## Lookup

`man 1 objdump`, `man 1 readelf`, `man 5 elf`, `info ld`, `man 1 gdb`.
