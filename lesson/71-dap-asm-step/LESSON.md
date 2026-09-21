# 71 — Step from harness into your `.s`

Lesson 33 practiced live ABI on `add3`. This dap drill repeats the multi-file
pattern with a fresh stub (`twiddle`: return `(a ^ b) + c`) and emphasizes the
launch rule that bites every harness+asm exercise: **`make` first**, then
**Launch existing** on `./twiddle_test`, never "Build current file." By the end
you should step from C into your own instructions and read the same register
story Track B taught on paper.

## What this lesson asks of you

Implement `twiddle` in assembly, break on the `twiddle(...)` call inside
`expect`, F11 into the `.s`, and watch register groups for `%rdi`/`%rsi`/`%rdx`
and the result in `%rax`. Optional `<leader>dm` only if you spill to the stack
(this stub need not). Terminate with `<leader>dt` when finished.

`TASK.md` is practice. Keys and honesty limits are taught here so the checklist
is judgement, not archaeology in `NVIM_DAP.md`.

## Why "Build current file" fails here

That config compiles the open buffer alone. Open `harness.c` and you lose
`twiddle.s`; open `twiddle.s` and you lose the harness. The Makefile already
knows both inputs and emits `./twiddle_test` with `-ggdb`. Your job is to launch
that artifact. The rejected habit from single-file toys — hit F5 and let the
editor rebuild — silently drops half the program.

## Call site → asm

F9 on the call in `harness.c` keeps C argument names visible. Note `a`,`b`,`c`
with Scopes / `<leader>de`, then F11. In Scopes register groups, confirm the
System V slots before your `xor` / `add` overwrite them. F10 may skip coarse
chunks once you are in asm depending on line maps — prefer F11 / continue, and
fall back to gdb `stepi` if you need instruction-precise tracing.

**F11 ≠ `stepi`.** Say it before you write lab notes. CodeLLDB's step-into is
source/line oriented. A three-instruction `twiddle` might blur under one or two
F11 presses. gdb `stepi` after `break twiddle` is the precise twin when you care
about each insn — same skill family as lesson 35's push watch.

## What "good" register reads look like

At entry: `%rdi`/`%rsi`/`%rdx` hold `a`/`b`/`c`. After your sequence, `%rax`
holds `(a ^ b) + c`. You do not need memory view unless you chose to spill; a
pure register implementation is fine and keeps the lesson focused on ABI regs.
If Scopes names registers differently under CodeLLDB, map them mentally to the
SysV names you already know — the bit patterns are what matter.

## Worked example

**The situation.** `expect(0x1, 0x2, 0x3, …)` calls `twiddle`. You are stopped
on the call line.

**Step 1 — caller values.** Hover shows 1, 2, 3. The rejected wrong reading is:
"I will discover args only from disassembly comments." Live values beat wishful
comments next to opcodes.

**Step 2 — enter `twiddle`.** `%rdi`/`%rsi`/`%rdx` match. Step until `%rax`
holds `(1 ^ 2) + 3`. The rejected wrong reading is: "F11 single-stepped each
machine instruction." It stepped at source granularity; verify with `stepi` if
you need insn cadence.

**Step 3 — finish.** Continue through remaining expects; `ok`; `<leader>dt`.
If a later expect fails, break again and compare entry regs to the failing
triple — ABI mistakes show up as wrong inputs, logic mistakes as wrong `%rax`
with right inputs.

## Distinctions worth keeping straight

- **Harness+`.s` via `make` vs single-file dap build** — real link versus
  incomplete binary.
- **Call-site F9 vs break on asm symbol** — C context versus label landing.
- **F11 vs `stepi`** — again, say it out loud.
- **Register result vs memory spill** — this stub can stay in GPRs.

## Check yourself

1. What exact launch choice do you make after `make`?
2. Which registers hold `twiddle`'s three args at entry?
3. Why is F11 not enough to claim you watched each asm insn?
4. Where do you set the first breakpoint if you want C names for `a`,`b`,`c`?

## Key takeaways

- Multi-file asm labs demand Launch existing on the Makefile binary.
- Break at the harness call, read C values, F11 into `.s`, confirm ABI regs.
- Keep F11 ≠ `stepi` honest; use gdb when instruction grain matters.
- `<leader>dt` ends the session cleanly when you are done observing.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `.s` + harness workflow; key map; F11 ≠ `stepi`
- `man 1 as`, `man 1 gdb` — `help info registers`, `help stepi`

Now open `TASK.md` and do the practice.
