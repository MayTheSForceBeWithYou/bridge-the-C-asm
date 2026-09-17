# 70 — dap memory view (Track C / dap)

## Goal
Inspect a stack `struct` and array as **bytes** with `<leader>dm`, correlate
with gdb `x/`.

## Do
1. Implement `fill_demo`. `make` → `./prog` prints `ok`.
2. Open `prog.c`. **F9** at the start of `fill_demo` (or just after the call
   returns in `main` — both work; after-return is easier for final layout).
3. `<leader>dc` → Launch existing → `./prog`.
4. From Scopes, note addresses of `pt` / `arr` / `p`.
5. `<leader>dm` — view memory at `&pt` (or `p`). Find little-endian `0x11`,
   `0x22`, and ASCII `ABC` in `tag`.
6. Memory-view `arr`; confirm `10,20,30,40` as 32-bit ints.
7. Add a **Watch** on `pt.x` or `arr[2]`; step if you broke inside `fill_demo`
   and watch values appear.

## gdb CLI (alternative)

```
gdb -q ./prog
(gdb) break main
(gdb) run
(gdb) next
# after fill_demo returns:
(gdb) x/16xb &pt
(gdb) x/4wd arr
(gdb) p pt
(gdb) continue
(gdb) quit
```

## Success
You matched dap memory bytes to the struct/array layout and (optionally) `x/`.
`./prog` prints `ok`.

## Refs
`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`
