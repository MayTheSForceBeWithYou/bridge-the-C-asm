# 32 — gdb args live (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `sum_idx` / `sum_ptr` so `{1,2,3,4,5}` sums to 15. `make` → `./prog`
   prints `ok` and exits 0.
2. Open `prog.c`. **F9** on `sum_idx`. `<leader>dc` → **"Launch existing
   executable"** → `./prog`.
3. On break: Scopes / `<leader>de` for args; note register equivalents of
   pointer and `5`. **`<leader>dm`** at the array pointer; confirm five ints.
4. Optionally **F11** into the body; optionally repeat for `sum_ptr`.
5. gdb alternative if you prefer: `break sum_idx`, `run`, `info registers rdi
   rsi`, `x/5wd $rdi`.

## Done when

- `./prog` prints `ok`, exit 0.
- You recorded live register / scope values that match the ABI (nvim-dap and/or
  gdb).

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, gdb `help info registers`, `help x`.
