# 52 — strip still runs (Track E)

## Goal
`strip` a copy: binary still runs; symbols gone; size shrinks.

## Do
1. Implement hello; `make`; `./prog` works.
2.
```
cp prog prog.stripped
nm prog > before.nm
size prog > before.size
strip prog.stripped
nm prog.stripped > after.nm || true
size prog.stripped > after.size
./prog.stripped
diff -u before.nm after.nm || true
cat before.size after.size
```
3. Contrast with exercise 38 (there: gdb by address; here: nm/size focus).

## Success
Stripped binary runs; nm symbols largely gone; you noted size change.

## Refs
`man 1 strip`, `man 1 nm`, `man 1 size`, `info binutils`
