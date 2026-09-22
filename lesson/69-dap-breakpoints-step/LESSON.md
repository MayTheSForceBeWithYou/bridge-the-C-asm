# 69 — nvim-dap keys on a tiny C program

Earlier Track C labs mixed gdb and dap while chasing ABI and memory questions.
This drill isolates the **editor keys** themselves on a small `classify` /
`accum` program so muscle memory matches `NVIM_DAP.md` — including the honest
limit that F10/F11/F12 are source-level steps, not `stepi`, and that a
conditional breakpoint is not a data watchpoint. By the end the checklist keys
should feel boring.

## What this lesson asks of you

Implement the stubs until `./prog` prints `ok`, then complete a checklist that
only counts if you used nvim-dap: toggle a breakpoint, launch the Makefile
binary correctly, step over/into/out, plant a conditional breakpoint, hover,
open the REPL briefly, and terminate. gdb is optional backup; the point is the
dap path.

`TASK.md` is practice. This file teaches what each key *means* so the checklist
is judgement, not archaeology.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | build `./prog` with `-ggdb` | Launch existing target |
| `make run` | run harness | `ok` when classify/accum pass |

`main` calls `classify(-3)`, `classify(0)`, `classify(7)`, then `accum(5)`. Your
stubs must make those expectations hold before the debug checklist matters.

## The key map (exact — do not invent)

| Key | Action | Rough gdb cousin |
| --- | ------ | ---------------- |
| F9 / `<leader>db` | toggle breakpoint (red marker) | `break` on line |
| F5 / `<leader>dc` | continue / start | `run` / `continue` |
| F10 / `<leader>do` | step over | `next` |
| F11 / `<leader>di` | step into | `step` |
| F12 / `<leader>dO` | step out | `finish` |
| `<leader>dB` | conditional breakpoint | `break … if …` |
| `<leader>de` | evaluate / hover | `print` |
| `<leader>dr` | DAP REPL | (lldb/gdb console) |
| `<leader>dt` | terminate | `kill` / quit session |
| `<leader>du` | toggle dap-ui | (TUI is different) |
| `<leader>dm` | memory float | `x/` (used more in 70) |

Launch with **"Launch existing executable"** → `./prog` (or a project shortcut
like Launch ./prog). Skip "Build current file" for this Makefile exercise.

### How to navigate the session

1. Open `prog.c` in the exercise directory.
2. Put the cursor on the first line of `classify` → **F9** (red marker visible).
3. `<leader>dc` → pick **"Launch existing executable"** → type `./prog`.
4. When stopped, confirm dap-ui (or `<leader>du`). Scopes should show locals/args.
5. Work the checklist in order — do not skip to `continue` until keys are done.

## Source step versus instruction step

CodeLLDB's F10/F11/F12 advance by **source line** (or statement), roughly like
gdb `next` / `step` / `finish`. They are **not** `stepi` / `nexti`.

| Key | Grain | Use when |
| --- | ----- | -------- |
| F10 | One source line, skip callees | Walk `classify` body |
| F11 | One source step, enter callees | Enter `accum` from `main` |
| F12 | Finish current frame | Leave `accum` back to `main` |
| gdb `stepi` | One machine insn | Not this checklist — honesty for later labs |

If a later lab needs every `push` visible, you already know to open gdb. Here,
claiming "I instruction-stepped with F11" would be false even if the program
behaved.

**Rejected wrong reading:** "F11 and `stepi` are the same because both say
step." Different grain, different tool.

## Conditional breakpoints vs watches vs data watchpoints

`<leader>dB` asks for a condition expression (for this lab: stop in `classify`
when `x == 0`). That is how you skip the negative and positive calls in `main`
and land only on the zero case — without hardware watchpoints.

| Mechanism | What it does | This lesson |
| --------- | ------------ | ----------- |
| F9 breakpoint | Always stop on that line | First stops in `classify` |
| `<leader>dB` condition | Stop on that line only when expr true | `x == 0` |
| dap-ui Watches | Display expr each stop | Optional; not required |
| gdb `watch x` | Trap on memory store | **Not** this drill (see lesson 34) |

**Recognition rule:** conditional breakpoints encode *when* relative to a
*line*. Data watchpoints encode *when memory changes*. Do not tell yourself you
practiced `watch` by using `<leader>dB`.

Continue until you see that selective stop; it trains intent ("stop when
interesting") rather than "stop every time."

## Launch existing vs Build current file

| Config | What it builds | Right for this dir? |
| ------ | -------------- | ------------------- |
| Launch existing → `./prog` | Nothing — runs what `make` made | **Yes** |
| Build current file and launch | `gcc -g` on the open buffer only | No — skips Makefile truth |
| Launch ./prog shortcut | Same as existing if wired in `launch.json` | Yes if present |

**Rejected wrong reading:** "F5 always does the right build." F5 runs the
*selected* configuration.

## Worked example

**The situation.** Stubs pass; `./prog` prints `ok`. You begin the checklist.

**Step 1 — first stop.** F9 on first line of `classify`, Launch existing. dap-ui
opens (or `<leader>du`). F10 a few lines; virtual text / Scopes update. The
rejected wrong reading is: "I will only use continue and read the final `ok`."
Keys unpracticed stay unfamiliar.

**Step 2 — into / out.** F9 on the `accum` call in `main` if needed; F11 into
`accum`; F12 back out. Confirm you can enter and leave a callee on purpose.
Note: F11 entered at *source* grain — if `accum` is tiny, you may land deep in
its body quickly; that is still not `stepi`.

**Step 3 — condition.** Clear or keep the old bp as you like; `<leader>dB` on
`classify` with `x == 0`. Continue until you stop only for zero. Hover
`<leader>de` on `x` / `n`. Open `<leader>dr` once so you know where it lives;
`<leader>dt` to end.

**Step 4 — checklist honesty.** Tick items only if you actually pressed the
keys. Green `ok` alone does not complete this lesson.


## What dap-ui is showing while you drill keys

Left panes typically include **scopes**, **watches**, **stacks**, and
**breakpoints**. Bottom panes include **repl**, **console**, and **memory**.
You do not need every pane for this checklist, but you should recognize them:

| Pane | Use on this drill |
| ---- | ----------------- |
| Breakpoints | Confirm F9 / `<leader>dB` entries appear |
| Scopes | See `x` / `n` update as you F10 |
| Stacks | See `classify` / `accum` / `main` frames while stepped in |
| Watches | Optional pin; not a substitute for `<leader>dB` |
| REPL (`<leader>dr`) | Open once — know it exists; no need to live there |

Virtual text overlays values on the source. If nothing updates while you mash
F5, you are not stepping — you are continuing past the interesting stops.

### Checklist anti-patterns

- Ticking F11/F12 without ever entering `accum` (no call-site bp / wrong stop).
- Using Build current file, getting a weird binary, and blaming the stubs.
- Setting `<leader>dB` with a condition that never holds (`x == 99`) and
  concluding conditional breakpoints "do not work."
- Calling F10/`stepi` interchangeably in notes.

Fix the anti-pattern, then tick the box.



## Mapping the checklist to competence

Each checklist tick corresponds to a judgement you will need later:

| Tick | Later payoff |
| ---- | ------------ |
| F9 red marker | You can plant stops without guessing syntax |
| Launch existing | Multi-file / Makefile labs stop breaking mysteriously |
| F10 / Scopes update | You can watch state without `printf` |
| F11 / F12 | You can enter and leave callees on purpose |
| `<leader>dB` | You can stop on interesting cases only |
| `<leader>de` / `<leader>dr` / `<leader>dt` | Hover, escape hatch, clean exit |

If you skip ticks because "I already know," the next asm harness lab will tax
you for the missing muscle memory.


## Distinctions worth keeping straight

- **F11 vs `stepi`** — source into versus one insn (gdb).
- **F9 vs `<leader>dB`** — always stop versus stop when expression true.
- **Conditional bp vs data watch** — line+predicate versus store trap.
- **Launch existing vs Build current file** — Makefile truth versus buffer-only.
- **Checklist done vs tests green** — both required; `ok` alone skips the keys.

## Check yourself

1. Which key steps out of the current function back to the caller?
2. What launch config do you pick after `make` produced `./prog`?
3. Is F11 guaranteed to execute a single assembly instruction?
4. How do you stop `classify` only when `x` is zero without editing `main`?
5. You add `x` to the Watches pane. Have you created a conditional breakpoint?
6. Which key terminates the debug session cleanly?

If fuzzy, re-read the key map table — then open `NVIM_DAP.md` only for spelling.

## Key takeaways

- Drill F9, F10–F12, `<leader>dc` / `dB` / `de` / `dr` / `dt` / `du` until they
  are boring.
- F10/F11/F12 are source-level; do not call them `stepi`.
- Conditional breakpoints encode "when," not only "where" — and are not data
  watches.
- Launch the binary `make` built; do not let the editor rebuild the wrong way.
- Checklist completion is part of competence; `ok` is necessary but not
  sufficient.

## Lookup (not the lesson)

- `NVIM_DAP.md` — full map and UI panel notes
- `.vscode/launch.json` — project launch configs

Now open `TASK.md` and do the practice.
