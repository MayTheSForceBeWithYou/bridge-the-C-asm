# 69 — dap breakpoints & step (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. Implement `classify` and `accum`. `make` → `./prog` prints `ok`.
2. Open `prog.c`. Use **nvim-dap** for the checklist (gdb optional).

### Checklist (tick in your notes)

- [ ] **F9** on first line of `classify` — red marker visible
- [ ] `<leader>dc` → **"Launch existing executable"** → `./prog`
- [ ] Session stops; dap-ui open (or `<leader>du`)
- [ ] **F10** a few lines; virtual text / Scopes update
- [ ] **F11** into `accum` from `main` (extra F9 on the call if needed)
- [ ] **F12** step out of `accum`
- [ ] `<leader>dB` on `classify` with `x == 0` — continue until only zero stops
- [ ] `<leader>de` on `x` / `n`
- [ ] `<leader>dr` briefly; `<leader>dt` terminate

## Done when

- `./prog` prints `ok` and the checklist is complete.

## Lookup

`NVIM_DAP.md`, `.vscode/launch.json`.
