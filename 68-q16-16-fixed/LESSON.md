# 68 — Q16.16 fixed-point without XMM
<!-- concepts: c.p5.bits.fixed -->

IEEE floats (exercise 16 / 63) use dedicated FP hardware and XMM registers on this ABI.
Fixed-point represents fractions with scaled integers — here Q16.16 means 16 fractional
bits in an `int32_t`, so `1.0` is `1 << 16`. Multiply and divide with 64-bit intermediates
and shifts, and the asm should show `imul`/`sar` style integer ops — not XMM float
math. This lesson builds that contrast deliberately.

## What this lesson asks of you

Represent Q16.16 as `int32_t`. Implement `q_mul` and `q_div`. Check that `1.5 * 2.0 ==
3.0` in this encoding. Inspect asm for absence of XMM float arithmetic in your
functions. Harness arithmetic checks pass; print `ok`.

## Q16.16 arithmetic

- Encode: `(int32_t)(value * 65536.0)` conceptually — in code prefer integer
  constructions like `(3 << 16) / 2` for 1.5 when teaching without float.
- Multiply: `(int32_t)(((int64_t)a * (int64_t)b) >> 16)` — product has 32 fractional
  bits before shifting back.
- Divide: `(int32_t)(((int64_t)a << 16) / b)` — careful with overflow and signs.

Exact formulas can vary with rounding policy; match the harness. The educational win is
integer-only asm and correct scaling.

## Worked example

**The situation.** `q_mul(q(1.5), q(2.0))` equals `q(3.0)`; `objdump` of your functions
shows integer multiplies/shifts; `ok`.

**Step 1 — encoding.** `1 << 16` is one. Build 1.5 and 2.0 as Q16.16 constants. The
rejected wrong reading is: "I will just use `float` and cast at the end." That fails the
XMM-free requirement for your mul/div bodies.

**Step 2 — mul/div with int64_t.** Without wide intermediates, `a * b` overflows 32 bits
before you shift. The rejected wrong reading is: "shift first then multiply to keep it
in 32 bits." That loses fraction bits; widen first for mul.

**Step 3 — asm check.** No `addss`/`mulss`/`xmm` float ops in *your* q functions
(printing may still use float elsewhere — focus on the fixed-point routines).

## Distinctions worth keeping straight

- **Q16.16 vs float** — scale factor versus IEEE fields (exercise 16 contrast).
- **Wide intermediate** — required for correct mul.
- **Integer asm vs XMM** — success criterion for this lab.
- **Rounding/overflow** — real fixed-point issues; harness defines the expected policy.

## Check yourself

1. What `int32_t` encoding represents `1.0` in Q16.16?
2. Why does multiplication need a 64-bit intermediate?
3. What asm evidence shows you avoided float ops?
4. How does this contrast with exercise 16?

## Key takeaways

- Fixed-point stores fractions in scaled integers.
- Q16.16 mul/div are shift + wide integer arithmetic.
- Asm should look integer, not XMM float.
- Useful when you need deterministic scaling without FP units/paths.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 16

Now open `TASK.md` and do the practice.
