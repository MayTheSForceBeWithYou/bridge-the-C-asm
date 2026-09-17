# nvim-dap cheat sheet (this machine)

Primary in-editor debugger for this grind. Adapter is **CodeLLDB** (installed
via mason). Your global config lives in `~/.config/nvim/lua/plugins/` —
`nvim-dap.lua`, `nvim-dap-ui.lua`, `nvim-dap-virtual-text.lua`.

Track C (and exercises 69–71) can be done in **nvim-dap** and/or **gdb CLI**.
Same skills; different UI.

## Prerequisites

Already on this machine:

- Neovim + Lazy plugins above
- mason ensures `codelldb`
- Project `.vscode/launch.json` (loaded by `dap.ext.vscode.load_launchjs`)

No extra install for the grind itself.

## Workflow (multi-file / Makefile)

`Build current file and launch` only `gcc -g`’s the *open buffer*. That breaks
`harness.c` + `.s`, or any `NAME` that is not the current file’s stem.

1. `make` (or `make -C <exercise>`) so `./prog` / `./add3_test` / etc. exists with `-ggdb`.
2. Open the main `.c` (or harness) in nvim from that exercise directory.
3. Set breakpoints with **F9** / `<leader>db`.
4. **F5** / `<leader>dc` → pick **"Launch existing executable"** → enter `./prog`
   (or the binary name from the TASK / Makefile `NAME`).
5. Optional shortcuts: **"Launch ./prog"** or a config from `.vscode/launch.json`.

Single-file throwaways can still use **"Build current file and launch"**.

## Keys (exact — do not invent)

| Key | Action |
|-----|--------|
| F5 / `<leader>dc` | continue / start |
| F9 / `<leader>db` | toggle breakpoint |
| `<leader>dB` | conditional breakpoint |
| F10 / `<leader>do` | step over |
| F11 / `<leader>di` | step into |
| F12 / `<leader>dO` | step out |
| `<leader>dt` | terminate |
| `<leader>dr` | DAP REPL |
| `<leader>de` | evaluate / hover |
| `<leader>du` | toggle dap-ui |
| `<leader>dm` | float memory viewer |

## UI panels (dap-ui)

Left: **scopes**, **watches**, **stacks**, **breakpoints**.

Bottom: **repl**, **console**, **memory**.

- Scopes: locals / args / registers (CodeLLDB exposes register groups).
- Watches: add expressions (e.g. `a[0]`, `x`).
- Stacks: backtrace while stopped / after a crash.
- Memory: `<leader>dm` or the bottom memory panel — paste a pointer from scopes
  (array / `buf` / SP) to view bytes.
- Virtual text: inline values while stepping (`nvim-dap-virtual-text`).

## Stepping: source line vs instruction

CodeLLDB / DAP **F10 / F11 / F12** usually step by **source line** (or
statement), not by single machine instruction.

- For **instruction-level** stepping (`stepi` / `nexti`), use **gdb CLI**, or
  try the DAP REPL with LLDB commands if you know them — do not assume F11 is
  `stepi`.
- When a TASK says “watch each `push` change `%rsp`”, prefer gdb `stepi` for
  precision; nvim-dap still works for call-site → step-into and memory view.

## Memory

1. Stop at a breakpoint where a pointer is live (Scopes).
2. `<leader>dm` — enter the address (hex from scopes / hover `<leader>de`).
3. Or use the bottom **memory** panel.
4. gdb alt: `x/8xg $rsp`, `x/16xb buf`, etc.

## `.s` + harness

1. `make` in the exercise dir.
2. Open `harness.c`.
3. Break on the **call site** in the harness (F9), or on the asm symbol name if
   debug info / symbol resolves.
4. `<leader>dc` → Launch existing (`./add3_test` or whatever `NAME` is).
5. At the call: F11 to step into asm; watch registers in Scopes / REPL.
6. If stepping into `.s` is fuzzy, break in harness just before the call, note
   args via `<leader>de`, then F11.

## gdb concepts → nvim-dap

| gdb | nvim-dap |
|-----|----------|
| `break foo` / `b foo` | F9 on line / symbol if resolved |
| `run` | F5 / `<leader>dc` (pick launch config) |
| `continue` | F5 / `<leader>dc` |
| `next` | F10 / `<leader>do` |
| `step` | F11 / `<leader>di` |
| `finish` | F12 / `<leader>dO` |
| `info locals` / `info args` | Scopes panel |
| `info registers` | Scopes (registers) / DAP REPL |
| `print expr` / `p expr` | `<leader>de` hover / Watches / REPL |
| `x/… addr` | `<leader>dm` memory view |
| `watch x` | try Watches + `<leader>dB`; **data watchpoints** may need gdb |
| `bt` | Stacks panel |
| `layout regs` / TUI | dap-ui + virtual text (not 1:1 with TUI) |
| `stepi` / `nexti` | **gdb** (honest fallback) |
| `break *0xADDR` | gdb primary; CodeLLDB REPL may allow address breaks |

## Honest tool limits

- **Data watchpoints** (`watch x`): gdb is reliable. nvim-dap has conditional
  breakpoints (`<leader>dB`) and watched *expressions*; hardware watchpoints
  via CodeLLDB UI are not guaranteed — use gdb for exercise 34’s `watch`.
- **Instruction step**: use gdb `stepi` when the TASK cares about each insn.
- **Stripped binaries / break by address**: gdb `break *0xADDR` is primary
  (exercise 38); try DAP REPL only as an experiment.
- **Crash → source line**: Stacks panel helps; still run `addr2line` (exercise 39).

## Refs

- This file; project `.vscode/launch.json`
- `:help dap.txt` (nvim-dap)
- Plugin docs via Lazy / GitHub for nvim-dap, nvim-dap-ui, nvim-dap-virtual-text
- gdb: `man 1 gdb`, `info gdb`, interactive `help` — see `REFS.md`
