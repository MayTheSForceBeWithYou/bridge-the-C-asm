# 19 — Inline asm bridge
<!-- concepts: c.p5.asm.inline -->

Extended Asm lets you drop a few assembler instructions inside C while still
letting GCC allocate registers and keep the surrounding function ABI-correct.
You write operands with constraints (letters like `r` for a general register,
`=` for output), and the compiler stitches your fragment into the larger
prologue/epilogue story from lesson 02. By the end of this lesson you should
implement a tiny `add2_asm`, explain your constraint choices, and know that
`asm volatile` affects *how the compiler treats the statement*, not magic CPU
volatility.

## What this lesson asks of you

Implement `long add2_asm(long a, long b)` with Extended Asm that adds the
operands and produces a C return value; keep `add2_c` as a pure-C twin; print
both. Skim `info gcc` Extended Asm *after* you understand the goal here — Lookup
is reference, not the teacher. Practice steps live in `TASK.md`.

## Operand anatomy (concept first)

An Extended Asm statement names:

- **Outputs** — C lvalues the asm will write; marked with `=` in the constraint.
- **Inputs** — C expressions read by the asm.
- **Clobbers** — registers or `"memory"` your asm destroys beyond the operands.

Constraint letters tell GCC *what kind of place* to pick. For a first add, a
register constraint (`r`) for inputs and an early-clobber or separate output
register is the usual pattern you will discover while iterating — match what
compiles and what the generated `.s` shows around your fragment.

Your fragment should assume AT&T syntax inside GNU as / GCC's asm templates
(`%0`, `%1` placeholders expand to the operands GCC chose).

## `volatile` on asm

`asm volatile` (or `__asm__ __volatile__`) tells GCC not to delete the asm even
if outputs look unused, and restricts some reordering relative to other
volatile accesses. It does **not** automatically freeze all surrounding memory
traffic. Experimenting with a nearby store to a global at `-O2` teaches more than
memorizing slogans — see whether your asm and that store keep the order you
expected, then adjust (for example with a `"memory"` clobber) if needed.

## Worked example

**The situation.** `add2_asm(2, 3)` and `add2_c(2, 3)` both print `5`. You inspect
`-O0` and `-O2` asm around the inline fragment.

**Step 1 — get a compiling version.** Focus on one output operand feeding the
`return`. The rejected wrong reading is: "inline asm replaces the need for a
C function prologue." GCC still generates a normal function wrapper unless it
inlines everything.

**Step 2 — read the generated `.s`.** See which registers `%0`/`%1` became and
how the result lands in the return register. The rejected wrong reading is:
"constraint `r` means `%rax` specifically." `r` means *some* GPR; GCC picks.

**Step 3 — compare to `add2_c`.** Same ABI; different body authorship.

**Step 4 — `-O2` reordering experiment.** Place a store to a global near the asm;
observe; adjust only if you understand why.

## Distinctions worth keeping straight

- **Template string vs final registers** — placeholders versus GCC's choices.
- **Output constraint vs return ABI** — you produce an operand; C `return`
  still uses the ABI channel.
- **`asm volatile` vs `volatile` object** — statement semantics vs object
  access rules (lesson 18).
- **Teaching fragment vs production asm** — keep the first example tiny.

## Check yourself

1. What does an `=` in a constraint signify?
2. Why might GCC still emit `push %rbp` around your two-instruction add?
3. Does `asm volatile` alone guarantee order against every nearby non-volatile
   store?
4. Where should you look up constraint letter spellings once you know you need
   them?

## Key takeaways

- Extended Asm binds C operands to asm placeholders with constraints.
- GCC still owns the function's ABI wrapper and register allocation.
- `asm volatile` limits deletion/reordering; it is not omniscient.
- Learn the mechanism here; use `info gcc` as Lookup for letter tables.

## Lookup (not the lesson)

- `info gcc` — Extended Asm (after this lesson)
- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
