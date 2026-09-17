# 65 — tagged pointer (Track G)

## Goal
Stash a 3-bit tag in the low bits of an 8-byte-aligned pointer; extract with
mask/shift; inspect `and`/`or` in asm.

## Do
1. Assume pointers from `malloc` are >=8-byte aligned.
2. `tag_ptr(p, tag)` / `untag_ptr` / `get_tag`.
3. `make asm` — find the masking ops.

## Success
Round-trip pointer + tag works; `./prog` prints `ok`.

## Refs
`man 1 gcc`, `man 1 objdump`
