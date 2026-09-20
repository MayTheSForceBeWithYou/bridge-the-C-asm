# 69 — nvim-dap keys on a tiny C program
<!-- concepts: c.p5.dap.break -->

Earlier Track C labs mixed gdb and dap while chasing ABI and memory questions.
This drill isolates the **editor keys** themselves on a small `classify` /
`accum` program so muscle memory matches `NVIM_DAP.md` — including the honest
limit that F10/F11/F12 are source-level steps, not `stepi`.

## What this lesson asks of you

Implement the stubs until `./prog` prints `ok`, then complete a checklist that
only counts if you used nvim-dap: toggle a breakpoint, launch the Makefile
binary correctly, step over/into/out, plant a conditional breakpoint, hover,
open the REPL briefly, and terminate. gdb is optional backup; the point is the
dap path.

## The key map (exact — do not invent)

| Key | Action |
| --- | --- |
| F9 / `<leader>db` | toggle breakpoint (red marker) |
| F5 / `<leader>dc` | continue / start |
| F10 / `<leader>do` | step over |
| F11 / `<leader>di` | step into |
| F12 / `<leader>dO` | step out |
| `<leader>dB` | conditional breakpoint |
| `<leader>de` | evaluate / hover |
| `<leader>dr` | DAP REPL |
| `<leader>dt` | terminate |
| `<leader>du` | toggle dap-ui |

Launch with **"Launch existing executable"** → `./prog` (or a project shortcut
like Launch ./prog). Skip "Build current file" for this Makefile exercise.

## Source step versus instruction step

CodeLLDB's F10/F11/F12 advance by **source line** (or statement), roughly like
gdb `next` / `step` / `finish`. They are **not** `stepi` / `nexti`. If a later
lab needs every `push` visible, you already know to open gdb. Here, claiming
"I instruction-stepped with F11" would be false even if the program behaved.

## Conditional breakpoints

`<leader>dB` asks for a condition expression (for this lab: stop in `classify`
when `x == 0`). That is how you skip the negative and positive calls in `main`
and land only on the zero case — without hardware watchpoints. Continue until
you see that selective stop; it trains intent ("stop when interesting") rather
than "stop every time."

## Worked example

**The situation.** `main` calls `classify(-3)`, `classify(0)`, `classify(7)`,
then `accum(5)`. You F9 the first line of `classify` and launch.

**Step 1 — first stop.** dap-ui opens (or `<leader>du`). F10 a few lines;
virtual text / Scopes update. The rejected wrong reading is: "I will only use
continue and read the final `ok`." Keys unpracticed stay unfamiliar.

**Step 2 — into / out.** F9 on the `accum` call in `main` if needed; F11 into
`accum`; F12 back out. Confirm you can enter and leave a callee on purpose.

**Step 3 — condition.** `<leader>dB` on `classify` with `x == 0`. Continue
until you stop only for zero. Hover `<leader>de` on `x` / `n`. Open `<leader>dr`
once so you know where it lives; `<leader>dt` to end.

## Distinctions worth keeping straight

- **F11 vs `stepi`** — source into versus one insn (gdb).
- **F9 vs `<leader>dB`** — always stop versus stop when expression true.
- **Launch existing vs Build current file** — Makefile truth versus buffer-only.
- **Checklist done vs tests green** — both required; `ok` alone skips the keys.

## Check yourself

1. Which key steps out of the current function back to the caller?
2. What launch config do you pick after `make` produced `./prog`?
3. Is F11 guaranteed to execute a single assembly instruction?
4. How do you stop `classify` only when `x` is zero without editing `main`?

## Key takeaways

- Drill F9, F10–F12, `<leader>dc` / `dB` / `de` / `dr` / `dt` / `du` until they
  are boring.
- F10/F11/F12 are source-level; do not call them `stepi`.
- Conditional breakpoints encode "when," not only "where."
- Launch the binary `make` built; do not let the editor rebuild the wrong way.

## Lookup (not the lesson)

- `NVIM_DAP.md` — full map and UI panel notes
- `.vscode/launch.json` — project launch configs

Now open `TASK.md` and do the practice.
