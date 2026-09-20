# 63 — IEEE-754 bits through a union
<!-- concepts: c.p5.bits.ieee -->

A `float` is not a tiny decimal object inside the CPU — it is a 32-bit pattern with a
sign bit, an 8-bit exponent field, and a 23-bit fraction (mantissa) field under
IEEE-754 binary32. Type-punning via a `union` of `float` and `uint32_t` lets you print
those bits as hex. This lesson insists you **predict on paper first**, then verify.

## What this lesson asks of you

On paper, for `1.0f` and `-2.5f`, write the expected 32-bit layout. Implement a union
pun, print hex, decode fields by hand, and match the harness's expected constants /
`ok`.

## Binary32 layout (reminder)

From bit 31 to bit 0:

- **sign** (1 bit): 0 positive, 1 negative
- **exponent** (8 bits): biased by 127 for normals
- **fraction** (23 bits): significand bits after the implicit leading 1 for normals

So `1.0f` is sign 0, exponent 127 (biased), fraction 0 → hex `0x3f800000`.
`-2.5f` needs sign 1 and the encoding of 2.5's significand/exponent — compute on paper
before running.

## Union punning in this track

```c
union { float f; uint32_t u; } pun;
pun.f = 1.0f;
printf("%08x\n", pun.u);
```

C's aliasing rules make some punning styles controversial; unions are the classic
educational device here. Exercise 64 compares `memcpy` as the standard-friendly
alternative — same bits if you are careful.

## Worked example

**The situation.** Your paper prediction for `1.0f` was `3f800000`; the program prints
the same; harness `ok`.

**Step 1 — paper for 1.0f.** Biased exponent 127, fraction 0. The rejected wrong reading
is: "1.0 should be hex 1 because the value is one." Hex here is the *bit pattern*, not
the integer value 1.

**Step 2 — paper for -2.5f.** Sign bit set; encode 2.5 = 1.25 × 2^1 → exponent bias
128, fraction for 0.25 in the significand bits. Finish the hex before compiling.

**Step 3 — run and decode.** Split printed hex into fields; confirm each.

## Distinctions worth keeping straight

- **Numeric value vs bit pattern hex** — different meanings of "what is this float."
- **Biased exponent vs power-of-two** — remember the +127 bias for normals.
- **Implicit leading 1** — not stored in the 23 bits for normals.
- **Union teaching device vs memcpy (64)** — both can yield the same bits.

## Check yourself

1. What are the widths of sign, exponent, and fraction in binary32?
2. Why is `1.0f`'s pattern `0x3f800000` rather than `0x00000001`?
3. What must you do before running the program in this exercise?
4. How do you recover the sign bit from a `uint32_t` hex value?

## Key takeaways

- IEEE-754 binary32 has a fixed field layout you can decode by hand.
- Paper-first prevents mistaking value for representation.
- Union punning exposes the bits for inspection.
- Harness constants confirm your decoding, not replace the paper work.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump` (optional asm peek)

Now open `TASK.md` and do the practice.
