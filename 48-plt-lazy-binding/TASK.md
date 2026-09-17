# 48 — PLT lazy binding (Track E)

## Goal
Find the `.plt` stub for `printf` and explain lazy binding from what you see.

## Do
1. Keep/adjust `hello.c` so it calls `printf`. Build dynamic `make`.
2. Inspect:
```
objdump -d -j .plt ./hello
objdump -d ./hello | less
readelf -r ./hello
readelf -d ./hello
```
3. Locate the `printf@plt` stub. What does it jump through? First call vs
   later calls (gdb: `break printf`, `run`, `disassemble`, `finish`, call
   again — optional but illuminating).
4. Cite `man 5 elf` / `info ld` for the mechanism name (PLT/GOT).

## Success
You named the PLT stub address and described lazy binding in your notes.

## Refs
`man 1 objdump`, `man 1 readelf`, `man 5 elf`, `info ld`, `man 1 gdb`
