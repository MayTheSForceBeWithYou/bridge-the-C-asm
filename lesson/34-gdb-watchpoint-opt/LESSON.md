# 34 — Why `watch` dies at `-O2`

A data watchpoint says: "stop when *this memory location* changes." At `-O0`,
a local like `x` in `bump` usually lives in a stack slot, so `watch x` has an
address to arm. At `-O2`, the same local is often promoted into a register for
its whole lifetime. There is no stable memory cell. The watch then fails to
fire, fires once on a spill you did not care about, or cannot be created the
way you expect — and the binary still computes the right answer. That is not a
gdb bug; it is optimization changing the *storage class* of your variable. By
the end you must keep **data watch**, **conditional breakpoint**, and **watched
expression** from collapsing into one vague "watch."

## What this lesson asks of you

Build the same `bump` at `-O0` and `-O2`, step both under nvim-dap so you see
locals appear or vanish, then use **gdb** for a real `watch x` comparison. The
judgement you are training: when a watch "does nothing," ask whether the value
still lives in memory before you blame the debugger.

`TASK.md` is practice. Hardware watchpoints are a gdb-primary skill on this
track; nvim-dap covers stepping and expressions.

## What `make` is naming for you

| You type | Rough equivalent | File afterward |
| -------- | ---------------- | -------------- |
| `make` / `make O=0` | debug build at `-O0` | `./prog` → copy to `prog.O0` |
| `make O=2` | same sources at `-O2` | `./prog` → copy to `prog.O2` |
| `make O=0 asm` / `make O=2 asm` | `gcc -S` views | `.s` listings to compare |

Typical TASK flow:

```text
make clean && make && cp prog prog.O0
make clean && make O=2 && cp prog prog.O2
```

Copy binaries so you can launch either without rebuilding over the other
mid-session.

## Three different "watch" ideas (do not merge them)

| Mechanism | What it does | Tool on this box |
| --------- | ------------ | ---------------- |
| **Data / hardware watchpoint** (`watch x`) | Trap when the *memory location* of `x` is stored to | **gdb** (required for this lab's comparison) |
| **Watched expression** (dap-ui Watches) | Re-display an expression each time you *already* stopped | nvim-dap Watches pane |
| **Conditional breakpoint** (`<leader>dB`) | Stop on a line only when an expression is true (e.g. `i == 3`) | nvim-dap; not a store trap |

On this machine, nvim-dap / CodeLLDB gives you Watches and `<leader>dB`. True
**hardware/data watchpoints** are **not reliably exposed** in the CodeLLDB UI.
For the `-O0` versus `-O2` watch comparison, use gdb. Still use nvim-dap to feel
locals disappear into registers when you launch `./prog.O2`.

**Recognition rule:** if you "added `x` to Watches" and stepped with F10, you
did **not** plant a data watchpoint. You planted a display. If you used
`<leader>dB` with `i == 3`, you planted a *conditional line break* — useful, but
a different instrument. Only gdb `watch x` answers "did a store to that cell
happen?"

**Rejected wrong reading:** "watchpoints only work at `-O0` forever." They work
when there is memory to watch; `-O0` merely makes that common for locals.

## What `-O2` does to `bump`

At `-O0`, expect a loop that loads/stores `x` and `i` around the increment. At
`-O2`, GCC may keep the accumulator in a register, fold the loop, or even turn
`bump(5)` into a constant `5` with almost no loop left. Debug info may still
*name* `x`, but `info locals` can show `<optimized out>`, and `watch x` has
nothing honest to bind to.

### How to see the storage-class change

1. `gdb -q ./prog.O0` → `break bump` → `run` → `info locals`.
2. Try `p &x` — at `-O0` you usually get a stack address.
3. `disassemble bump` — look for stores to stack slots (`movl %…, -N(%rbp)`).
4. Repeat on `./prog.O2`. Expect fewer stack stores, `<optimized out>`, or a
   near-empty body. `p &x` may fail or be meaningless.

Also compare `make O=0 asm` versus `make O=2 asm` if you want the compiler's
plan in `.s` form. The rejected wrong reading after a silent `-O2` watch is:
"gdb broke." Prefer: "the variable is not a memory object anymore."

## nvim-dap keys still matter (even when gdb owns `watch`)

| Key | Role in this lab |
| --- | ---------------- |
| F9 | Break on `bump` |
| F5 / `<leader>dc` | Launch existing → `./prog.O0` or `./prog.O2` |
| F10 | Step the loop; watch Scopes / Watches *display* |
| `<leader>dB` | Conditional line break (not a data watch) |
| `<leader>dt` | Terminate before switching binaries |
| `<leader>du` | Recover dap-ui |

## Worked example

**The situation.** You have `prog.O0` and `prog.O2`.

**Session A (`-O0`).** `break bump`, `run`, `watch x`, `continue`. Expect hits
as the loop stores a new `x`. Continue a few times; quit when you have seen the
pattern. The rejected wrong reading is: "every continue must hit exactly once
per source line." Watchpoints fire on *stores to that location*, which may not
line up 1:1 with how you read the C loop.

**Session B (`-O2`).** Same commands on `./prog.O2`. Often the watch never fires
usefully, or gdb complains / binds oddly. Check `info locals` and `disassemble
bump`. Write one sentence in your notes that mentions *registers* or
*optimized-out storage* — not "gdb is broken."

**nvim-dap pass.** F9 on `bump`, Launch existing → `./prog.O0`, add Watch on
`x`, F10 through the loop — remember that Watch is a *display*. Terminate,
launch `./prog.O2`, notice how Scopes treat locals. Optional: `<leader>dB` with
a condition on `i` if debug info cooperates — still not a data watchpoint.


## How to run the gdb watch sessions without losing the plot

Stay disciplined about *which binary* is under test. A common junior failure is
to `watch x` on `./prog` after a rebuild flipped `-O0` to `-O2` without
noticing. That is why the TASK copies to `prog.O0` and `prog.O2`.

Suggested gdb cadence for session A:

```text
gdb -q ./prog.O0
(gdb) break bump
(gdb) run
(gdb) info locals
(gdb) p &x
(gdb) watch x
(gdb) continue
# note hit; continue a few more times
(gdb) disassemble bump
(gdb) quit
```

Then the same skeleton on `./prog.O2`. When `p &x` fails or `info locals` shows
`<optimized out>`, you already have the explanation for a silent watch — write
that sentence *before* you start blaming hardware watchpoint limits.

### What a watch hit looks like (recognition)

When `watch x` fires at `-O0`, gdb typically reports that a write occurred and
shows old/new values (wording varies by version). That stop is about a **store
to a memory cell**. It is not the same UI as dap-ui highlighting a Watch row
while you F10. If you only ever saw the dap Watches pane update, you practiced
a different skill.

**Rejected wrong reading:** "I put `x` in Watches and stepped; that counts as
the gdb watch comparison." It does not. The TASK requires the gdb sessions.

## Connecting `<leader>dB` without confusing yourself

Conditional breakpoints remain useful under `-O2` when debug info still knows
`i` or a line in the loop: you can stop when `i == 3` even if `x` is a
register. That is a *control-flow* filter, not proof that `x` has an address.
Use it to explore optimized builds; do not cite it as evidence that data
watchpoints work at `-O2`.



## Why correct answers make this lesson slippery

Optimized `bump` can still return the mathematically right value while
destroying watchability. Juniors equate "program correct" with "debugger
features work." This lab breaks that equation on purpose. When a production
watch fails, ask: *does this value still live in memory at this PC?* before
opening bug trackers against gdb.



## Optional asm/listing comparison without turning TASK into a lecture

`make O=0 asm` and `make O=2 asm` write `.s` files you can search for `bump:`.
At `-O0`, expect obvious stores. At `-O2`, expect a thinner function — maybe a
lea/add trick or near-constant return. You are not grading GCC; you are
collecting evidence that storage changed. Pair that evidence with the gdb
`watch` outcome in your notes.


## Distinctions worth keeping straight

- **Data watchpoint vs watched expression vs conditional bp** — trap on store
  versus display on stop versus stop-when-predicate.
- **`-O0` stack local vs `-O2` register** — addressable cell versus GPR.
- **Correct program vs watchable program** — `ok` does not imply `watch` works.
- **nvim-dap for stepping / gdb for `watch`** — honest tool split on this box.
- **`<optimized out>` vs wrong answer** — storage missing from debug view, not
  necessarily a logic bug.

## Check yourself

Close this file and answer from memory:

1. Why can `watch x` fail to fire at `-O2` even though `bump` returns the right
   value?
2. You add `x` to dap-ui Watches and step with F10. Have you created a hardware
   data watchpoint? What did you create instead?
3. Name the nvim-dap key for a *conditional* breakpoint, and give one example
   condition for this lab.
4. What does `<optimized out>` in `info locals` tell you about storage?
5. Which tool is required for the `-O0` / `-O2` `watch x` comparison on this
   track?
6. After a silent `-O2` watch, what listing do you open before blaming gdb?

If fuzzy, re-read the three-way table — then `help watch` for spelling only.

## Key takeaways

- Watchpoints track **memory locations**; optimization often removes those
  locations for locals.
- At `-O2`, silent watches are a storage-class lesson, not random tool failure.
- Watched expressions and `<leader>dB` conditions are not substitutes for gdb
  `watch`.
- Use gdb for hardware `watch`; use nvim-dap for line steps, Scopes, and
  conditions.
- Always pair the watch experiment with a disassembly glance at both opt
  levels.

## Lookup (not the lesson)

- `NVIM_DAP.md` — honest watchpoint limits; `<leader>dB`, Watches, F10
- `man 1 gdb` — then `help watch`
- `man 1 gcc` — `-O0` / `-O2` (Makefiles use `O=`)

Now open `TASK.md` and do the practice.
