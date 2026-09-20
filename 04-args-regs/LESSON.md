# 04 — Argument registers
<!-- concepts: c.p2.stack.byvalue, c.p5.abi.argregs -->

When one C function calls another, arguments have to live somewhere the callee
can find them. On x86-64 System V AMD64 (GNU/Linux), the first six *integer /
pointer* arguments travel in fixed registers; further arguments spill to the
stack. There is no ABI man page in the verified reference set for this track —
you discover the convention by reading what `gcc -S` emits. By the end of this
lesson you should name those six registers from evidence and know where a
seventh argument and a scalar return value appear.

## What this lesson asks of you

Implement tiny functions of one, two, six, and seven `long` parameters, call them
from `main` with distinctive constants, and read the assembly. Do not look up an
ABI PDF. The listing is the teacher; Lookup entries are only for tool flags.

## How to read a call site

In `main` at `-O0`, GCC typically materializes each constant into a register (or
stack slot) and then places arguments where the callee expects them *just before*
`call`. Distinct constants (1, 2, 3, … or clearer hex patterns) make it obvious
which argument landed where: if you see `$7` moved into a register immediately
before `call f7`, that register is carrying the seventh *logical* argument's
setup — unless it is part of stack preparation; always read a few lines of
context.

After `call` returns, scalar integer results show up where the *caller* consumes
them — almost always by reading `%rax` / `%eax` (lesson 05 expands returns).

## The six-register pattern

Build `f1` through `f6` first. For each arity, note which registers are loaded
at the call site and which registers the callee reads on entry. You should see a
stable assignment for arguments 1–6. Write it down from *your* `.s` file. That
list *is* your ABI note for the rest of Track A.

When you add `f7`, six registers will still be used for the first six arguments;
the seventh will be passed in memory. Look for a store relative to `%rsp` before
the `call`, and a matching load from a positive stack offset in the callee
(often `N(%rbp)` after a prologue, or an offset from `%rsp`).

## Callee view versus caller view

The caller *writes* argument registers / stack slots. The callee *reads* them —
often after shuffling into locals at `-O0`. Do not be surprised if the callee
immediately spills `%rdi` into `-8(%rbp)` and then works from the stack. That is
pedagogical `-O0` behavior, not a different ABI.

## Worked example

**The situation.** You implement `long f6(long a,b,c,d,e,f)` returning a sum, and
`long f7(..., long g)` likewise. `main` calls both with constants `1…6` and
`1…7`.

**Step 1 — inspect `main`'s call to `f6`.** List the six moves into registers.
Order them by argument position using the constant values. The rejected wrong
reading is: "whatever register is mentioned first in the file is argument one."
Argument identity comes from *which constant* / *which C parameter*, not from
textual order of unrelated instructions.

**Step 2 — inspect `f6`'s body.** Confirm it reads the same registers (or their
spills). The rejected wrong reading is: "the callee picks any registers it
likes for incoming args." Incoming locations are fixed by convention; after that,
temps are free.

**Step 3 — inspect `f7`.** Find the seventh constant on the stack at the call
site and the matching load in `f7`. The rejected wrong reading is: "the seventh
argument also gets a dedicated register on this ABI." If your listing shows
stack traffic for `g`, believe the listing.

**Step 4 — note the return path.** Where does `main` take the sum from after
`call`? Remember that location for lesson 05.

## Distinctions worth keeping straight

- **Argument registers vs return register** — incoming versus outgoing.
- **First six in regs vs seventh on stack** — arity changes the picture.
- **ABI home vs `-O0` spill** — spilling to a local does not redefine the ABI.
- **Discovering vs memorizing from a PDF** — this track requires discovery.

## Check yourself

1. How would you experimentally find which register carries argument 3?
2. Where should you look for argument 7 in the caller before `call`?
3. After `f6` returns a `long`, which register should you inspect first in the
   caller?
4. Why might `f6` at `-O0` store `%rdi` to `-8(%rbp)` even though `%rdi` already
   held the argument?

## Key takeaways

- SysV AMD64 passes the first six integer/pointer args in fixed registers.
- Further args pass on the stack; your `.s` shows the offsets.
- Scalar integer returns are consumed from a known return register.
- Derive the map from `gcc -S` / `objdump -d` with distinctive constants.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- No ABI man page is in the verified set — use compiler output.

Now open `TASK.md` and do the practice.
