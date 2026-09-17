# 69 — dap breakpoints & step (Track C / dap)

## Goal
Practice the exact nvim-dap keys on a tiny C program. No instruction-level
`stepi` claims — F10/F11/F12 are source-level steps.

## Do
1. Implement `classify` and `accum` (see stubs). `make` → `./prog` prints `ok`.
2. Open `prog.c`. Use **only** nvim-dap for the checklist below (gdb optional).

### Student checklist (tick in your notes)

- [ ] **F9** on the first line of `classify` — red breakpoint marker visible
- [ ] `<leader>dc` → **"Launch existing executable"** → `./prog` (or Launch ./prog)
- [ ] Session stops on the breakpoint; dap-ui opens (or `<leader>du`)
- [ ] **F10** step over a few lines; virtual text / Scopes update
- [ ] **F11** step into `accum` from `main` (set another F9 on the call if needed)
- [ ] **F12** step out of `accum` back to `main`
- [ ] `<leader>dB` conditional breakpoint on `classify` with condition `x == 0`
      — continue until it stops only for the zero case
- [ ] `<leader>de` hover on `x` / `n`
- [ ] `<leader>dr` open REPL briefly; `<leader>dt` terminate

## Success
`./prog` prints `ok`, and your checklist is complete. No solution file in-repo.

## Refs
`NVIM_DAP.md`, `.vscode/launch.json`
