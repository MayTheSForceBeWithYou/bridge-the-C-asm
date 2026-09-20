# 34 — Why `watch` dies at `-O2`

A data watchpoint says: "stop when *this memory location* changes." At `-O0`,
a local like `x` in `bump` usually lives in a stack slot, so `watch x` has an
address to arm. At `-O2`, the same local is often promoted into a register for
its whole lifetime. There is no stable memory cell. The watch then fails to
fire, fires once on a spill you did not care about, or cannot be created the
way you expect — and the binary still computes the right answer. That is not a
gdb bug; it is optimization changing the *storage class* of your variable.

## What this lesson asks of you

Build the same `bump` at `-O0` and `-O2`, step both under nvim-dap so you see
locals appear or vanish, then use **gdb** for a real `watch x` comparison. The
judgement you are training: when a watch "does nothing," ask whether the value
still lives in memory before you blame the debugger.

`TASK.md` is practice. Hardware watchpoints are a gdb-primary skill on this
track; nvim-dap covers stepping and expressions.

## What a watchpoint actually tracks

gdb's `watch x` (after you are in a frame where `x` exists) asks the debug
hardware or software layer to trap stores to the *location* of `x`. Expression
watches in an IDE that re-evaluate `x` each stop are a different feature: they
display a value when you already stopped; they do not necessarily plant a
hardware trap on a store.

On this machine, nvim-dap / CodeLLDB gives you:

- **Watched expressions** in dap-ui (handy mirrors of `x` / `i`).
- **Conditional breakpoints** via `<leader>dB` (e.g. stop when `i == 3`).

True **hardware/data watchpoints** (`watch x` in gdb) are **not reliably
exposed** in the CodeLLDB UI. For the `-O0` versus `-O2` watch comparison, use
gdb. Still use nvim-dap to feel locals disappear into registers when you launch
`./prog.O2`.

## What `-O2` does to `bump`

At `-O0`, expect a loop that loads/stores `x` and `i` around the increment. At
`-O2`, GCC may keep the accumulator in a register, fold the loop, or even turn
`bump(5)` into a constant `5` with almost no loop left. Debug info may still
*name* `x`, but `info locals` can show `<optimized out>`, and `watch x` has
nothing honest to bind to.

Disassemble both builds (`disassemble bump` in gdb, or `make O=0 asm` versus
`make O=2 asm`). The rejected wrong reading after a silent `-O2` watch is: "gdb
broke." Prefer: "the variable is not a memory object anymore."

## Worked example

**The situation.** You copied `prog` to `prog.O0` and `prog.O2` as the TASK
describes.

**Session A (`-O0`).** `break bump`, `run`, `watch x`, `continue`. Expect hits
as the loop stores a new `x`. Continue a few times; quit when you have seen the
pattern. The rejected wrong reading is: "every continue must hit exactly once
per source line." Watchpoints fire on *stores to that location*, which may not
line up 1:1 with how you read the C loop.

**Session B (`-O2`).** Same commands on `./prog.O2`. Often the watch never
fires usefully, or gdb complains / binds oddly. Check `info locals` and
`disassemble bump`. Explain the miss in terms of registers / optimized-out
storage — not in terms of "watchpoints only work at `-O0` forever." They work
when there is memory to watch; `-O0` merely makes that common for locals.

**nvim-dap pass.** F9 on `bump`, launch `./prog.O0`, Watch `x`, F10 through the
loop. Terminate (`<leader>dt`), launch `./prog.O2`, notice how Scopes treat
locals. Conditional bp (`<leader>dB`) can still stop on `i` if debug info
cooperates — that is not the same as a data watchpoint.

## Distinctions worth keeping straight

- **Data watchpoint vs watched expression** — trap on store versus display on
  stop.
- **`-O0` stack local vs `-O2` register** — addressable cell versus GPR.
- **Correct program vs watchable program** — `ok` does not imply `watch` works.
- **nvim-dap for stepping / gdb for `watch`** — honest tool split on this box.

## Check yourself

1. Why can `watch x` fail to fire at `-O2` even though `bump` returns the right
   value?
2. Name one nvim-dap feature that is *not* a substitute for gdb `watch`.
3. What does `<optimized out>` in `info locals` tell you about storage?
4. You need to stop when `i == 3` under dap without a hardware watch. What key
   sequence plants that?

## Key takeaways

- Watchpoints track **memory locations**; optimization often removes those
  locations for locals.
- At `-O2`, silent watches are a storage-class lesson, not random tool failure.
- Use gdb for hardware `watch`; use nvim-dap for line steps, Scopes, and
  `<leader>dB` conditions.
- Always pair the watch experiment with a disassembly glance at both opt
  levels.

## Lookup (not the lesson)

- `NVIM_DAP.md` — honest watchpoint limits; `<leader>dB`, Watches, F10
- `man 1 gdb` — then `help watch`
- `man 1 gcc` — `-O0` / `-O2` (Makefiles use `O=`)

Now open `TASK.md` and do the practice.
