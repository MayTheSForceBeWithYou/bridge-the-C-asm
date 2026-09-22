# 13 — Recursion and stack growth

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

## What `make` is naming for you

| You type | Why |
| -------- | --- |
| `make` (default `-O0`) | Keep frames measurable — do **not** start with `O=2` |
| `make asm` / `make disasm` | Map `push` / `sub` to the stride you will measure |
| gdb on `./prog` | Live `%rsp` ruler |

Avoid `-O2` for the first pass: tail-call rewriting (lesson 14) or algorithmic
conversion to a loop can collapse the growth you came to measure.

## What one call pushes

On entry to a call already in progress, a `call` instruction has pushed the
return address (8 bytes). The callee prologue may push `%rbp` (another 8) and
subtract immediate space for locals / alignment. Summing those pieces explains
why `%rsp` drops by a predictable stride each recursive entry when the frame
shape is stable.

| Piece | Typical size | Where you see it |
| ----- | ------------ | ---------------- |
| Return address | 8 | implied by `call`; visible as stack contents |
| Saved `%rbp` | 8 | `pushq %rbp` in prologue |
| Local/align space | `N` | `subq $N, %rsp` if present |

At the base case, no further call occurs; epilogues + `ret` pop frames in LIFO
order. Watching `%rsp` rise on the way out confirms the story.

### Estimating stride from disassembly alone

1. `disassemble fac` in gdb (or open `prog.lst` and search `<fac>:`).
2. Note `push %rbp` (+8) and any `sub $N,%rsp` (+N).
3. Add the implicit return address from the caller's `call` (+8).
4. Predict: each nested entry should move `%rsp` by about `16+N` (plus any other
   pushes). Then measure and reconcile.

**Rejected wrong reading:** "`%rsp` changed because factorial math pushed the
product." The product usually lives in registers or a local slot; the big stride
is frame setup + return address.

## gdb as a ruler — concrete session shape

You are not learning gdb theater — you are using it as a ruler against the mental
model from lessons 02–03. A minimal shape:

```text
$ gdb -q ./prog
(gdb) break fac
(gdb) run
(gdb) info registers rsp rbp
(gdb) disassemble fac
(gdb) stepi
(gdb) info registers rsp rbp
… repeat stepi through prologue …
(gdb) continue          # hit fac again at next depth
(gdb) info registers rsp rbp
```

| Command | What you use it for here |
| ------- | ------------------------ |
| `break fac` | stop at every entry (recursive hits included) |
| `info registers rsp rbp` | read the rulers |
| `stepi` | one instruction — watch prologue push/sub |
| `disassemble fac` | legend for what you are stepping |
| `continue` | run to the next breakpoint hit (deeper call) |

Record `%rsp` at a stable point after prologue for depth *d*, continue to depth
*d+1*, subtract. That difference is your measured stride.

**Rejected wrong reading:** "disassembly is unrelated to register numbers." It is
the map legend for the `%rsp` changes you measure.

### `%rbp` chaining

At `-O0`, saved `%rbp` often links frames: current `%rbp` points at the saved
previous `%rbp`. Following that chain is how backtraces work. After a nested
entry, compare `x/1gx $rbp` (saved old frame pointer) to the previous hit's
`%rbp` — they should match.

## Worked example

**The situation.** `fac(5)` recurses to `fac(0)` or `fac(1)` depending on your
base case. You break on `fac` and continue / stepi through several nested hits.

**Step 1 — record `%rsp` at a stable point after prologue.** Continue to the
next recursive entry and record again. The difference is your per-call stride.

**Step 2 — `disassemble fac`.** Match the stride to `push` / `sub $N,%rsp`.

**Step 3 — note `%rbp` chaining.** Confirm saved frame pointers link.

**Step 4 — unwind a few `finish` or `continue` steps.** Watch `%rsp` recover as
calls return.


## Recording sheet (use while in gdb)

```text
depth  rsp              rbp              notes
0      ________________ ______________   after prologue of first hit
1      ________________ ______________   after continue
2      ________________ ______________
stride (rsp0-rsp1) = ________ bytes
predicted from disasm = ________ bytes
```

Fill at least three depths. If stride is not constant, you stepped at
inconsistent points (sometimes mid-prologue) — standardize on "first instruction
after `mov %rsp,%rbp`" or similar.

### Why lesson 14 waits

If you pass `-O2` now, you may see `fac` become a loop and `%rsp` barely move.
That is a different lesson. Finish the nesting measurement at `-O0` first.


## Instruction-level prologue watch

Useful `stepi` sequence after hitting `break fac`:

1. Note `%rsp` on entry (return address already pushed by `call`).
2. `stepi` through `push %rbp` — `%rsp` drops 8; saved rbp on stack.
3. `stepi` through `mov %rsp,%rbp` — `%rbp` updates; `%rsp` unchanged.
4. `stepi` through `sub $N,%rsp` if present — `%rsp` drops N.
5. Record post-prologue `%rsp` as your depth sample.

That beats sampling at a random instruction mid-body where further pushes may
have happened.


## Tie-back to lessons 02–03

The bytes you measure are the same kinds of bytes those lessons named: saved
`%rbp`, optional `sub` for locals, plus the return address from `call`. Recursion
does not invent a new stack physics — it only multiplies the frame you already
learned to read.

## Distinctions worth keeping straight

- **Return address vs frame pointer vs locals** — three stack residents.
- **Recursive depth vs frame size** — depth multiplies stride.
- **`-O0` measurable growth vs `-O2` possible collapse** — choose the build for
  the lesson's goal.
- **`stepi` vs `step`** — instruction vs source line; use `stepi` for prologue.
- **gdb inspection vs guessing from C** — measure.

## Check yourself

1. What does `call` place on the stack before the callee runs?
2. Why prefer `-O0` when studying recursive stack growth?
3. How do you estimate frame stride from disassembly alone?
4. What should happen to `%rsp` as recursive calls return?
5. Which gdb command prints `%rsp` and `%rbp` together?
6. Why might `continue` be better than endless `stepi` once you understand the
   prologue?

## Key takeaways

- Recursion nests stack frames; `%rsp` falls then rises.
- Per-call stride comes from return address + prologue allocation.
- gdb `stepi` + `info registers` makes the stride visible; disassembly is the
  legend.
- Keep `-O0` until you understand the unoptimized frame story.

## Lookup (not the lesson)

- `man 1 gdb`; inside gdb: `help disassemble`, `help stepi`, `help info registers`
- `man 1 objdump`

Now open `TASK.md` and do the practice.
