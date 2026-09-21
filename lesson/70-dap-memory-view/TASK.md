# 70 — dap memory view (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `fill_demo`. `make` → `./prog` prints `ok`.
2. **F9** at start of `fill_demo` or just after it returns in `main`.
3. `<leader>dc` → Launch existing → `./prog`.
4. From Scopes, note addresses of `pt` / `arr` / `p`. **`<leader>dm`** at `&pt`
   (or `p`): find LE `0x11`, `0x22`, ASCII `ABC`. Memory-view `arr` for
   `10,20,30,40`. Optional Watch on `pt.x` / `arr[2]`.
5. gdb alternative: after `fill_demo` returns — `x/16xb &pt`, `x/4wd arr`,
   `p pt`.

## Done when

- You matched dap memory bytes to the struct/array layout (and optionally
  `x/`). `./prog` prints `ok`.

## Lookup

`NVIM_DAP.md`, `man 1 gdb`, gdb `help x`.
