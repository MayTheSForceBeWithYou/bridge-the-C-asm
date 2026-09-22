# 63 — IEEE-754 bits through a union

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

## Binary32 field table (memorize widths)

| Field | Width | Position (31…0) |
| ----- | ----- | --------------- |
| sign | 1 | bit 31 |
| exponent | 8 | 30–23 (bias 127 for normals) |
| fraction | 23 | 22–0 |

## Paper first: `1.0f`

Sign 0, biased exponent 127, fraction 0 → `0x3f800000`.

**Rejected wrong reading:** hex should be `1` because the value is one — hex is the *bit
pattern*.

## Paper first: `-2.5f`

Sign 1; encode magnitude 2.5 = 1.25×2¹ → biased exponent 128; fraction holds the
significand bits after the implicit 1. Finish hex *before* compiling.

## Union pun shape

```c
union { float f; uint32_t u; } pun;
pun.f = 1.0f;
printf("%08x\n", pun.u);
```

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| paper predictions | written before run |
| `make` / `./prog` | hex matches paper; harness `ok` |

## Navigation when decoding printed hex

Split `0xAARRRRRR`-style output into sign / exp / frac fields by shifts and masks; confirm
each against paper.

## Worked example

**The situation.** Your paper prediction for `1.0f` was `3f800000`; the program prints
the same; harness `ok`.

**Step 1 — paper for 1.0f.** Biased exponent 127, fraction 0. The rejected wrong reading
is: "1.0 should be hex 1 because the value is one." Hex here is the *bit pattern*, not
the integer value 1.

**Step 2 — paper for -2.5f.** Sign bit set; encode 2.5 = 1.25 × 2^1 → exponent bias
128, fraction for 0.25 in the significand bits. Finish the hex before compiling.

**Step 3 — run and decode.** Split printed hex into fields; confirm each.

## Field extraction from a `uint32_t` pattern

```text
sign = (u >> 31) & 1
exp  = (u >> 23) & 0xff
frac =  u        & 0x7fffff
```

Use these masks on your printed hex to verify paper predictions. Exercise 64 shows
`memcpy` as an alternative pun with the same bits.

## Distinctions worth keeping straight

- **Numeric value vs bit pattern hex** — different meanings of "what is this float."
- **Biased exponent vs power-of-two** — remember the +127 bias for normals.
- **Implicit leading 1** — not stored in the 23 bits for normals.
- **Union teaching device vs memcpy (64)** — both can yield the same bits.

## Deeper worked navigation (IEEE union)

- Paper first.
- 1.0f = 0x3f800000.
- Hex is bit pattern, not numeric value 1.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| paper | 1.0f and -2.5f |
| printf %08x | matches paper |
| field split | sign/exp/frac |

### Ordered navigation moves

1. Predict.
2. Pun.
3. Decode fields.
4. Match harness.

### Rejected wrong readings (keep beside the artifact)

- 1.0f hex is 1.
- skip paper.
- %f shows IEEE fields.

### Tool-line decoding reminders

- 1|8|23 widths
- bias 127
- masks/shifts

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (63)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - hex=1 for 1.0f
- skipped paper
- confused %f with fields

    For each tick: write the *recognition* fix (which register/column/anchor) in one line.
    That diary is how this track sticks.

    ## Makefile → command → file (recap)

    | You type | Produces / runs | Open next |
    | -------- | --------------- | --------- |
    | `make` / `make bin` | exercise binary | run it; note exit status |
    | `make clean` | removes objects | before changing `O=` / flags |
    | `make asm` / `make disasm` (if any) | listing view | search the label you care about |
    | tools in Lookup | field dumps | decode columns, do not skim blobs |

    Remember: a disasm target usually *views* bytes already linked — it is not a new
    mysterious compile stage (lesson 01's `.lst` rule).

## Check yourself

1. What are the widths of sign, exponent, and fraction in binary32?
2. Why is `1.0f`'s pattern `0x3f800000` rather than `0x00000001`?
3. What must you do before running the program in this exercise?
4. How do you recover the sign bit from a `uint32_t` hex value?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- IEEE-754 binary32 has a fixed field layout you can decode by hand.
- Paper-first prevents mistaking value for representation.
- Union punning exposes the bits for inspection.
- Harness constants confirm your decoding, not replace the paper work.

## Field-decoding recap for exercise 63

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump` (optional asm peek)

Now open `TASK.md` and do the practice.
