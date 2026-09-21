# 32 — Live args in registers

Track A taught you that on System V AMD64 the first integer or pointer argument
lands in `%rdi` and the second in `%rsi`. That was a static claim about a calling
convention. This lesson makes the claim *observable*: you stop a running process
inside `sum_idx` and read those registers (or the Scopes panel that shows the
same ABI slots) before the function body has a chance to clobber them.

## What this lesson asks of you

Build a tiny summing program with debug info, break at the callee entry, and
confirm live that the array pointer and length match what `main` passed. The
competence is not "I pressed F9." It is: when a later bug looks like "wrong
arguments," you already know how to *see* the ABI at the stop, in nvim-dap or
gdb, without guessing from the source alone.

`TASK.md` is the practice. This file is the teaching. `NVIM_DAP.md` and `man 1
gdb` are lookup for key spellings once you know what you are doing.

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

## nvim-dap versus gdb for the same stop

Both tools attach to the same kind of debuggee. The workflow difference is UI.

In nvim-dap on this machine (CodeLLDB):

- **F9** toggles a breakpoint on the current line (red marker).
- **`<leader>dc`** (or F5) starts or continues. For Makefile builds, pick
  **"Launch existing executable"** and enter `./prog` — do not use "Build
  current file," which only compiles the open buffer and skips the real link.
- Scopes (dap-ui; toggle with `<leader>du`) show locals, args, and register
  groups.
- **`<leader>de`** evaluates / hovers an expression under the cursor.
- **`<leader>dm`** opens a memory float: paste the pointer value of `a` and read
  the five `int`s as bytes or words.
- **F11** steps *into* at source-line granularity. That is not `stepi`
  (instruction step). For single-instruction walks, use gdb — later lessons
  rely on that honesty.

In gdb CLI the same story is `break sum_idx`, `run`, `info registers rdi rsi`,
optionally `x/5wd $rdi` to dump the array as words.

## Call-site versus callee break

Breaking on the first line of `sum_idx` stops after the `call` has transferred
control and after the return address is already on the stack. The argument
registers should still hold caller-prepared values until your prologue or body
overwrites them. Breaking on the *call site* in `main` lets you inspect locals
in the caller first, then F11 / `step` into the callee. Either works; callee
entry is the cleanest place to read `rdi`/`rsi` for this exercise.

## Worked example

**The situation.** `main` builds `{1,2,3,4,5}` and calls `sum_idx(a, 5)`. You
implemented the sum so `./prog` prints `ok`. You set F9 on `sum_idx`, launched
`./prog`, and stopped.

**Step 1 — read the ABI slots.** In Scopes or `info registers`, `%rdi` (or the
CodeLLDB name for it) is a heap-or-stack address, not `5`. `%rsi`'s low 32 bits
are `5`. The rejected wrong reading is: "Scopes shows `a` and `n` as C names, so
registers do not matter." Named locals are a debug-info convenience layered on
top of the same machine state. When debug info lies or optimization eats
locals, registers remain the ground truth.

**Step 2 — memory-view the array.** Take the pointer value, open `<leader>dm`
(or `x/5wd $rdi`). You should see little-endian `1,2,3,4,5`. The rejected wrong
reading is: "the five ints live in registers." Only the *pointer* and *count*
are in registers at entry; the payload is in memory.

**Step 3 — step a few times.** Watch whether your implementation keeps `n` in a
register or spills it. You are training eyes, not optimizing yet.

## Distinctions worth keeping straight

- **Static ABI diagram vs live registers** — lecture versus process.
- **Launch existing vs Build current file** — multi-file / Makefile truth versus
  single-buffer toss-off.
- **F11 vs `stepi`** — source/line step versus one machine instruction.
- **Args in regs vs args in memory** — the pointer is in `%rdi`; the array bytes
  are elsewhere.

## Check yourself

1. At entry to `sum_idx(a, 5)`, which register holds the length?
2. Why is "Build current file and launch" the wrong nvim-dap config for this
   directory's `make`-built `./prog`?
3. You see `a` correctly in Scopes but want to confirm the five ints. What do
   you open, and what address do you paste?
4. Does F11 guarantee you advance one assembly instruction?

## Key takeaways

- System V integer/pointer args are live in `%rdi`, `%rsi`, … at callee entry;
  confirm them under a debugger, do not only recite the table.
- nvim-dap Scopes / `<leader>de` / `<leader>dm` and gdb `info registers` / `x`
  are two UIs on one skill.
- Makefile binaries need **"Launch existing executable"** after `make`.
- F11 is not `stepi`; keep that limit honest before Track C's stack and
  watchpoint labs.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F9, F10–F12, `<leader>dc` / `dm` / `de` / `du`, launch configs
- `man 1 gdb` — then `help info registers`, `help x`
- `man 1 gcc` — `-ggdb` (already in this track's Makefiles)

Now open `TASK.md` and do the practice.
