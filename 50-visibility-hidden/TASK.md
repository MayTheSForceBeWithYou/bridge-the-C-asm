# 50 — -fvisibility=hidden (Track E)

## Goal
See which symbols vanish from the dynamic symbol table with
`-fvisibility=hidden`.

## Do
1. Build default and hidden:
```
make
gcc -ggdb -O0 -fvisibility=hidden -o prog.hidden prog.c
nm ./prog
nm ./prog.hidden
readelf --dyn-syms ./prog
readelf --dyn-syms ./prog.hidden
```
2. Which functions remain exported? What does `visibility` change for a
   non-shared executable vs if this were a `.so`? (Reason from `nm` /
   dyn-syms output.)

## Success
You documented symbol differences before/after `-fvisibility=hidden`.

## Refs
`man 1 nm`, `man 1 readelf`, `man 1 gcc`, `info gcc`
