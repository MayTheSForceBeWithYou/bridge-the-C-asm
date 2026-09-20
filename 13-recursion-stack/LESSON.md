# 13 — Recursion and stack growth
<!-- concepts: c.p2.stack.recursion, c.p2.stack.frame -->

Each active function call owns a *stack frame*: return address, saved registers
(often `%rbp`), and locals. Recursion makes that concrete — `fac(n)` calling
`fac(n-1)` nests frames until the base case unwinds them. By the end of this
lesson you should watch `%rsp` move in gdb, estimate bytes per call from
disassembly, and name what gets saved each entry at `-O0`.

## What this lesson asks of you

Implement a straightforward recursive `long fac(long n)` (classic multiply-after-
call form is fine). Build at `-O0`, then use gdb to break on `fac`, step
instructions, and inspect `%rsp` / `%rbp` across nested entries. `TASK.md`
lists a minimal session.

## What one call pushes

On entry to a call already in progress, a `call` instruction has pushed the
return address (8 bytes). The callee prologue may push `%rbp` (another 8) and
subtract immediate space for locals / alignment. Summing those pieces explains
why `%rsp` drops by a predictable stride each recursive entry when the frame
shape is stable.

At the base case, no further call occurs; epilogues + `ret` pop frames in LIFO
order. Watching `%rsp` rise on the way out confirms the story.

## gdb as a ruler

`break fac`, `run`, then `stepi` through prologue instructions while `info
registers rsp rbp` after each interesting step. `disassemble fac` shows you
what you are stepping through. You are not learning gdb theater — you are using
it as a ruler against the mental model from lessons 02–03.

Avoid `-O2` for the first pass: tail-call rewriting (lesson 14) can collapse the
growth you came to measure.

## What not to optimize away yet

If you pass `-O2` too early, you may accidentally study lesson 14's topic
(tail calls) or inlining instead of stack growth. Keep the first measurement at
`-O0` with a deliberately non-tail recursive body so each call truly nests.

## Worked example

**The situation.** `fac(5)` recurses to `fac(0)` or `fac(1)` depending on your
base case. You break on `fac` and continue / stepi through several nested hits.

**Step 1 — record `%rsp` at a stable point after prologue.** Continue to the
next recursive entry and record again. The difference is your per-call stride.
The rejected wrong reading is: " `%rsp` changed because factorial math pushed
the product." The product usually lives in registers or a local slot; the big
stride is frame setup + return address.

**Step 2 — `disassemble fac`.** Match the stride to `push` / `sub $N,%rsp`. The
rejected wrong reading is: "disassembly is unrelated to register numbers." It is
the map legend.

**Step 3 — note `%rbp` chaining.** At `-O0`, saved `%rbp` often links frames;
following that chain is how backtraces work.

**Step 4 — unwind a few `finish` or `continue` steps.** Watch `%rsp` recover.

## Distinctions worth keeping straight

- **Return address vs frame pointer vs locals** — three stack residents.
- **Recursive depth vs frame size** — depth multiplies stride.
- **`-O0` measurable growth vs `-O2` possible collapse** — choose the build for
  the lesson's goal.
- **gdb inspection vs guessing from C** — measure.

## Check yourself

1. What does `call` place on the stack before the callee runs?
2. Why prefer `-O0` when studying recursive stack growth?
3. How do you estimate frame stride from disassembly alone?
4. What should happen to `%rsp` as recursive calls return?

## Key takeaways

- Recursion nests stack frames; `%rsp` falls then rises.
- Per-call stride comes from return address + prologue allocation.
- gdb `stepi` + `info registers` makes the stride visible.
- Keep `-O0` until you understand the unoptimized frame story.

## Lookup (not the lesson)

- `man 1 gdb`; inside gdb: `help disassemble`, `help stepi`, `help info registers`
- `man 1 objdump`

Now open `TASK.md` and do the practice.
