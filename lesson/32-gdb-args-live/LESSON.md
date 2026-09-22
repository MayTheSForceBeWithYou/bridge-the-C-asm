# 32 — Live args in registers

Track A taught you that on System V AMD64 the first integer or pointer argument
lands in `%rdi` and the second in `%rsi`. That was a static claim about a calling
convention. This lesson makes the claim *observable*: you stop a running process
inside `sum_idx` and read those registers (or the Scopes panel that shows the
same ABI slots) before the function body has a chance to clobber them. By the end
you should tell a register pane value from a memory address, and know which
nvim-dap key opens each kind of truth.

## What this lesson asks of you

Build a tiny summing program with debug info, break at the callee entry, and
confirm live that the array pointer and length match what `main` passed. The
competence is not "I pressed F9." It is: when a later bug looks like "wrong
arguments," you already know how to *see* the ABI at the stop, in nvim-dap or
gdb, without guessing from the source alone.

`TASK.md` is the practice. This file is the teaching. `NVIM_DAP.md` and `man 1
gdb` are lookup for key spellings once you know what you are looking for.

## Why "live" is a different kind of truth

`gcc -S` shows the compiler's *plan* for moving arguments. A debugger shows the
*machine state* after the caller executed that plan. Those usually agree at
`-O0` with `-ggdb`. They can disagree in spirit at high optimization: a formal
parameter may never sit in a stack slot, or may be dead by the time you stop on
a later line. For this exercise you stay at the default debug build so entry
state is readable.

Integer and pointer args for the first six positions use, in order: `%rdi`,
`%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`. Returns go in `%rax` (and `%rdx` for a
128-bit pair — not needed here). Floating-point args use XMM registers; ignore
them for this lab.

So for `sum_idx(const int *a, int n)` at a breakpoint on the first useful
instruction of the function, expect `%rdi` to hold the address of the array and
`%rsi` to hold `5` (zero-extended or sign-extended into the 64-bit register
depending on how the caller prepared it — you care that the low bits are `5`).

## What `make` is naming for you

| You type | Rough equivalent | File / check afterward |
| -------- | ---------------- | ---------------------- |
| `make` / `make bin` | compile + link with `-ggdb -O0` | `./prog` |
| `make run` | run the harness | prints `ok`, exit 0 |

Default `CFLAGS` here include `-ggdb` so Scopes and named locals work. Rebuild
after edits before launching the debugger — a stale `./prog` is a common false
"ABI bug."

## How to read Scopes: names, registers, and addresses

When dap-ui opens (toggle with `<leader>du` if it did not), the left **Scopes**
pane is several different *kinds* of information stacked together. Treat them as
columns of meaning, not one blob of hex:

| What you see | Example shape | What it is |
| ------------ | ------------- | ---------- |
| Named arg / local | `a = 0x7fffffff…`, `n = 5` | Debug-info convenience: C names mapped onto machine state |
| Register group entry | `rdi = 0x7fffffff…`, `rsi = 0x5` | The ABI slots themselves — ground truth when names lie |
| A pointer's *value* | `0x7fffffffe1a0` | An **address** of memory elsewhere — not the payload |
| The integers in the array | (not in registers at entry) | Bytes at that address — open memory view |

**Recognition rule — register pane vs address:** if the value looks like a
large hex pointer (`0x7fff…` / heap-looking) and the C type is a pointer, you
are looking at *where* the array lives. The five `int`s are *not* that hex
number; they live at that address. If you paste `5` into memory view, you are
looking at the wrong thing.

**Rejected wrong reading:** "Scopes shows `a` and `n` as C names, so registers
do not matter." Named locals are a debug-info convenience layered on top of the
same machine state. When debug info lies or optimization eats locals, registers
remain the ground truth.

## nvim-dap keys for this stop (exact)

On this machine (CodeLLDB), use these — do not invent alternates:

| Key | Action for this lab |
| --- | ------------------- |
| **F9** / `<leader>db` | Toggle breakpoint on the current line (red marker) |
| **F5** / `<leader>dc` | Start or continue |
| **"Launch existing executable"** | After `make`, enter `./prog` |
| **F10** / `<leader>do` | Step over (source line) |
| **F11** / `<leader>di` | Step into (source line — **not** `stepi`) |
| **F12** / `<leader>dO` | Step out |
| `<leader>de` | Evaluate / hover expression under cursor |
| `<leader>dm` | Float memory viewer — paste a **pointer** address |
| `<leader>du` | Toggle dap-ui panes |
| `<leader>dt` | Terminate session |

**Launch existing, not "Build current file."** Makefile builds link the real
`NAME`. "Build current file and launch" only `gcc -g`'s the open buffer and can
skip the real link. Always `make` first, then Launch existing → `./prog`.

In gdb CLI the same story is `break sum_idx`, `run`, `info registers rdi rsi`,
optionally `x/5wd $rdi` to dump the array as words.

## Call-site versus callee break

Breaking on the first line of `sum_idx` stops after the `call` has transferred
control and after the return address is already on the stack. The argument
registers should still hold caller-prepared values until your prologue or body
overwrites them. Breaking on the *call site* in `main` lets you inspect locals
in the caller first, then F11 / `step` into the callee. Either works; callee
entry is the cleanest place to read `rdi`/`rsi` for this exercise.

## Memory-view anchors for the array

1. Stop at `sum_idx` entry with Scopes showing `a` (or `%rdi`).
2. Copy the pointer *value* — the address — not the digit `5` from `n`.
3. `<leader>dm` → paste that address into the float viewer (or use the bottom
   memory panel).
4. Read five little-endian 32-bit words: for `{1,2,3,4,5}` expect bytes like
   `01 00 00 00  02 00 00 00  …` (or word view `1, 2, 3, 4, 5`).

gdb twin: `x/5wd $rdi`. Same bytes, different keyboard.

**Rejected wrong reading:** "the five ints live in registers." Only the
*pointer* and *count* are in registers at entry; the payload is in memory.

## F11 is not `stepi`

**F10 / F11 / F12** under CodeLLDB advance by **source line** (or statement),
roughly like gdb `next` / `step` / `finish`. They are **not** `stepi` /
`nexti`. One F11 may execute several machine instructions that belong to one C
line. For single-instruction walks, use gdb — later lessons (stack after call,
watch each `push`) rely on that honesty. Claiming "I instruction-stepped with
F11" is false even when the program behaved.

## Worked example

**The situation.** `main` builds `{1,2,3,4,5}` and calls `sum_idx(a, 5)`. You
implemented the sum so `./prog` prints `ok`. You set F9 on `sum_idx`, launched
`./prog` via Launch existing, and stopped.

**Step 1 — read the ABI slots.** In Scopes, open the register group as well as
Locals/Args. `%rdi` (or CodeLLDB's name for it) is a heap-or-stack address, not
`5`. `%rsi`'s low 32 bits are `5`. Cross-check: named `a` should equal `%rdi`;
named `n` should match the low bits of `%rsi`. If names and regs disagree, trust
regs and ask whether you stopped after the body already clobbered them.

**Step 2 — memory-view the array.** Take the pointer value from `%rdi` / `a`,
open `<leader>dm` (or `x/5wd $rdi`). Confirm little-endian `1,2,3,4,5`. If you
see junk, you pasted `n` or a stale address — re-copy from Scopes at *this*
stop.

**Step 3 — step a few times (source grain).** F10/F11 through the body. Watch
whether your implementation keeps `n` in a register or spills it. You are
training eyes, not optimizing yet. Optionally repeat for `sum_ptr` if the TASK
asks.

**Step 4 — optional gdb twin.** `break sum_idx`, `run`, `info registers rdi
rsi`, `x/5wd $rdi`. Same ABI story without the editor UI.


## Optional gdb transcript (same recognition rules)

```text
gdb -q ./prog
(gdb) break sum_idx
(gdb) run
(gdb) info registers rdi rsi
(gdb) x/5wd $rdi
(gdb) print n
(gdb) continue
```

`info registers` is the register pane. `x/5wd $rdi` is the memory view.
`print n` is the named-local convenience. Keep those three roles straight when
you switch back to nvim-dap's Scopes / `<leader>dm` / `<leader>de`.


## Distinctions worth keeping straight

- **Static ABI diagram vs live registers** — lecture versus process.
- **Register pane value vs memory address vs payload bytes** — ABI slot versus
  pointer versus what `<leader>dm` shows at that pointer.
- **Launch existing vs Build current file** — multi-file / Makefile truth versus
  single-buffer toss-off.
- **F11 vs `stepi`** — source/line step versus one machine instruction.
- **Args in regs vs args in memory** — the pointer is in `%rdi`; the array bytes
  are elsewhere.
- **Named Scopes vs ground-truth GPRs** — convenience versus what survives when
  debug info thins out.

## Check yourself

Close this file and answer from memory, then verify at a live stop:

1. At entry to `sum_idx(a, 5)`, which register holds the length? Which holds the
   array pointer?
2. In Scopes you see `a = 0x7fffffffe1a0` and `n = 5`. Which of those do you paste
   into `<leader>dm` to see the five ints? Why not the other?
3. Why is "Build current file and launch" the wrong nvim-dap config for this
   directory's `make`-built `./prog`?
4. Does F11 guarantee you advance one assembly instruction?
5. Named local `a` looks right but you want ABI confirmation. Which Scopes group
   do you open, and which two register names matter first?
6. What exact launch menu wording do you pick after `make`?

If any answer is fuzzy, re-read the matching section — do not open `man gdb`
yet. When you need the spelling of `info registers`, *then* look it up.

## Key takeaways

- System V integer/pointer args are live in `%rdi`, `%rsi`, … at callee entry;
  confirm them under a debugger, do not only recite the table.
- Register panes show ABI slots; pointer *values* are addresses; payload bytes
  need `<leader>dm` / `x/` at those addresses.
- nvim-dap Scopes / `<leader>de` / `<leader>dm` and gdb `info registers` / `x`
  are two UIs on one skill.
- Makefile binaries need **"Launch existing executable"** after `make`.
- F11 is not `stepi`; keep that limit honest before Track C's stack and
  watchpoint labs.

## Lookup (not the lesson)

Use these when you already know the question:

- `NVIM_DAP.md` — F9, F10–F12, `<leader>dc` / `dm` / `de` / `du`, launch configs
- `man 1 gdb` — then `help info registers`, `help x`
- `man 1 gcc` — `-ggdb` (already in this track's Makefiles)

Now open `TASK.md` and do the practice.
