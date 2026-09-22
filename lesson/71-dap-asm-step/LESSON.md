# 71 — Step from harness into your `.s`

Lesson 33 practiced live ABI on `add3`. This dap drill repeats the multi-file
pattern with a fresh stub (`twiddle`: return `(a ^ b) + c`) and emphasizes the
launch rule that bites every harness+asm exercise: **`make` first**, then
**Launch existing** on `./twiddle_test`, never "Build current file." By the end
you should step from C into your own instructions, read the register story Track
B taught on paper, and keep **F11 ≠ `stepi`** honest in your notes.

## What this lesson asks of you

Implement `twiddle` in assembly, break on the `twiddle(...)` call inside
`expect`, F11 into the `.s`, and watch register groups for `%rdi`/`%rsi`/`%rdx`
and the result in `%rax`. Optional `<leader>dm` only if you spill to the stack
(this stub need not). Terminate with `<leader>dt` when finished.

`TASK.md` is practice. Keys and honesty limits are taught here so the checklist
is judgement, not archaeology in `NVIM_DAP.md`.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | assemble `twiddle.s`, compile `harness.c`, link | `./twiddle_test` |
| `make run` | run harness | prints `ok` |

`NAME` is `twiddle_test` — that is the Launch existing path.

## Why "Build current file" fails here

That config compiles the open buffer alone. Open `harness.c` and you lose
`twiddle.s`; open `twiddle.s` and you lose the harness. The Makefile already
knows both inputs and emits `./twiddle_test` with `-ggdb`. Your job is to launch
that artifact.

| Habit | Result |
| ----- | ------ |
| `make` then Launch existing → `./twiddle_test` | Full program, debug info |
| F5 with Build current file on `harness.c` | Missing asm → link/undef or wrong binary |
| F5 with Build current file on `twiddle.s` | Missing harness |

**Rejected habit:** hit F5 and let the editor rebuild — silently drops half the
program.

## Call site → asm (navigation)

1. Open `harness.c`.
2. Search for `twiddle(` inside `expect`.
3. **F9** on that call line.
4. `<leader>dc` → **"Launch existing executable"** → `./twiddle_test`.
5. Note `a`,`b`,`c` with Scopes / `<leader>de`.
6. **F11** into the callee.

F9 on the call site keeps C argument names visible. In Scopes register groups,
confirm the System V slots before your `xor` / `add` overwrite them. F10 may
skip coarse chunks once you are in asm depending on line maps — prefer F11 /
continue, and fall back to gdb `stepi` if you need instruction-precise tracing.

### F11 ≠ `stepi` (say it before lab notes)

CodeLLDB's step-into is source/line oriented. A three-instruction `twiddle`
might blur under one or two F11 presses. gdb `stepi` after `break twiddle` is
the precise twin when you care about each insn — same skill family as lesson
35's push watch.

| Tool | Grain | When |
| ---- | ----- | ---- |
| F11 | Source/line into `.s` | Arrive; see regs at entry / after coarse steps |
| gdb `stepi` | One insn | Watch each `xor` / `add` / `ret` |
| F10 | Step over line | May be awkward inside `.s` line maps |

**Rejected wrong reading:** "F11 single-stepped each machine instruction." It
stepped at source granularity; verify with `stepi` if you need insn cadence.

## What "good" register reads look like

| Moment | `%rdi` / `%rsi` / `%rdx` | `%rax` |
| ------ | ------------------------ | ------ |
| Entry to `twiddle` | `a` / `b` / `c` | often stale |
| After correct body | may be clobbered | `(a ^ b) + c` |

You do not need memory view unless you chose to spill; a pure register
implementation is fine and keeps the lesson focused on ABI regs. If Scopes names
registers differently under CodeLLDB, map them mentally to the SysV names you
already know — the bit patterns are what matter.

**Recognition rule — register pane vs address:** for `twiddle(long,long,long)`,
entry regs hold small integer values (or whatever the harness passed), not
pointer anchors for `<leader>dm`. Contrast lesson 32 (pointer + array in
memory) and lesson 70 (struct bytes).

## Keys (exact)

| Key | Role here |
| --- | --------- |
| F9 | Call-site bp in `harness.c` |
| F5 / `<leader>dc` | Launch existing → `./twiddle_test` |
| F11 | Step into `twiddle` |
| F10 / F12 | Optional coarse steps / step out |
| `<leader>de` | Hover `a`,`b`,`c` |
| `<leader>dm` | Only if you spilled |
| `<leader>dt` | End session |
| `<leader>du` | Recover dap-ui |

## Worked example

**The situation.** `expect(0x1, 0x2, 0x3, …)` calls `twiddle`. You are stopped
on the call line.

**Step 1 — caller values.** Hover shows 1, 2, 3. The rejected wrong reading is:
"I will discover args only from disassembly comments." Live values beat wishful
comments next to opcodes.

**Step 2 — enter `twiddle`.** `%rdi`/`%rsi`/`%rdx` match. Step until `%rax`
holds `(1 ^ 2) + 3` (= `0`). If entry regs were wrong, fix the launch/break
first; if entry was right and `%rax` is wrong, fix the asm sequence.

**Step 3 — finish.** Continue through remaining expects; `ok`; `<leader>dt`.
If a later expect fails, break again and compare entry regs to the failing
triple — ABI mistakes show up as wrong inputs, logic mistakes as wrong `%rax`
with right inputs.

**Step 4 — optional gdb.** `break twiddle`, `run`, `info registers`, `stepi` —
same ABI story with insn grain when F11 blurred the body.


## Relating `twiddle` to lessons 33 and 35

Lesson 33's `add3` was the same multi-file ritual with different opcodes. If
Launch existing / call-site F9 / entry regs already feel familiar, treat this
as a *retention drill* with a new stub — not a brand-new concept pile.

Lesson 35's honesty about F11 vs `stepi` applies unchanged. If your lab notes
for `twiddle` claim per-instruction observation, they need a gdb `stepi`
transcript. F11 notes should say you entered the callee and sampled regs at
source grain.

### Debugging a failing expect with registers

Harness prints which triple failed. Break again on the call, Launch existing,
stop, read regs:

| Observation | Likely cause |
| ----------- | ------------ |
| Entry `%rdi/%rsi/%rdx` mismatch harness args | Wrong break frame, wrong binary, or clobbered before you looked |
| Entry regs correct, `%rax` wrong after body | Bug in `xor`/`add`/`lea` sequence or wrong result register |
| Never enter `.s` on F11 | Break on wrong line; or Build-current binary without asm |

Photograph entry first — always. Logic bugs and ABI bugs demand different
fixes; the register pane is how you tell them apart.



## A minimal correct `twiddle` shape (recognition, not a paste solution)

The TASK forbids shipping you a finished body — but you must recognize what
*kind* of instruction story belongs here so you can tell ABI success from
opcode success. Conceptually, entry has `a` in `%rdi`, `b` in `%rsi`, `c` in
`%rdx`; a correct sequence xor's `a` with `b`, adds `c`, leaves the result in
`%rax`, then `ret`. Whether you destroy `%rdi` along the way or use a
three-register dance is your choice; the live check is entry photo + final
`%rax`.

Do not copy a solution from another exercise's `add3` without changing the
ops — wrong opcode sequences still "look ABI-correct" at entry and fail the
harness afterward.



**Rejected wrong reading:** "Build current file is fine if I have `harness.c`
open because that is where `main` lives." The harness without `twiddle.s` is
not this program. `make` + Launch existing is non-negotiable for harness+`.s`
directories.


## Distinctions worth keeping straight

- **Harness+`.s` via `make` vs single-file dap build** — real link versus
  incomplete binary.
- **Call-site F9 vs break on asm symbol** — C context versus label landing.
- **F11 vs `stepi`** — again, say it out loud.
- **Register result vs memory spill** — this stub can stay in GPRs.
- **Wrong entry regs vs wrong `%rax`** — calling convention versus your opcodes.

## Check yourself

1. What exact launch choice do you make after `make`?
2. Which registers hold `twiddle`'s three args at entry?
3. Why is F11 not enough to claim you watched each asm insn?
4. Where do you set the first breakpoint if you want C names for `a`,`b`,`c`?
5. For `(1 ^ 2) + 3`, what should `%rax` hold when the body is correct?
6. When is `<leader>dm` necessary for this stub?

If fuzzy, re-read the launch table and F11≠`stepi` section before `NVIM_DAP.md`.

## Key takeaways

- Multi-file asm labs demand Launch existing on the Makefile binary.
- Break at the harness call, read C values, F11 into `.s`, confirm ABI regs.
- Keep F11 ≠ `stepi` honest; use gdb when instruction grain matters.
- Register panes carry this stub's story; memory view is optional.
- `<leader>dt` ends the session cleanly when you are done observing.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `.s` + harness workflow; key map; F11 ≠ `stepi`
- `man 1 as`, `man 1 gdb` — `help info registers`, `help stepi`

Now open `TASK.md` and do the practice.
