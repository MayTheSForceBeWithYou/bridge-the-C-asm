# 32 — gdb args live (nvim-dap + gdb) (Track C)

## Goal
Confirm System V AMD64 integer args arrive in registers **while the program
runs**. Self-contained stubs so debugging works even if exercise 07 is unfinished.

## nvim-dap (primary)

1. Implement `sum_idx` / `sum_ptr` so `{1,2,3,4,5}` sums to 15.
2. `make` in this directory (builds `./prog` with `-ggdb`).
3. Open `prog.c`. Put the cursor on `sum_idx` and hit **F9** (breakpoint).
4. **`<leader>dc`** (or F5) → pick **"Launch existing executable"** → enter `./prog`
   (do **not** use "Build current file" — that path is for single-file toss-offs;
   here `make` already linked `prog`).
5. On break: open Scopes (dap-ui auto-opens; toggle with `<leader>du`). Inspect
   args / registers. Hover with `<leader>de` on `a` / `n`.
6. **F11** step into; watch virtual text and Scopes update.
7. `<leader>dm` — memory view at the array pointer address from Scopes (the
   `a` / `rdi`-equivalent pointer). Confirm the five ints.
8. Continue; optionally break on `sum_ptr` the same way.

## gdb CLI (alternative)

```
gdb -q ./prog
(gdb) break sum_idx
(gdb) run
(gdb) info registers rdi rsi
(gdb) stepi
(gdb) stepi
(gdb) info registers
(gdb) continue
(gdb) break sum_ptr
(gdb) continue
(gdb) info registers rdi rsi
(gdb) quit
```

At the first stop in `sum_idx`: does `rdi` hold the array pointer and `rsi` hold `5`?

## Success
`./prog` prints `ok` and exits 0. You recorded live register / scope values
(nvim-dap and/or gdb).

## Refs
`NVIM_DAP.md`, `man 1 gdb`, gdb `help stepi`, `help info registers`, `man 1 gcc`
