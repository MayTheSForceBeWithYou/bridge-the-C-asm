# 68 — Q16.16 fixed-point without XMM

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

## Q16.16 encoding

| Concept | Encoding |
| ------- | -------- |
| `1.0` | `1 << 16` |
| mul | `(int32_t)(((int64_t)a * (int64_t)b) >> 16)` |
| div | `(int32_t)(((int64_t)a << 16) / b)` (mind overflow/sign) |

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make` | `1.5 * 2.0 == 3.0` in Q16.16; harness `ok` |
| `objdump -d` your q_* functions | `imul`/`sar`-style integer ops; no XMM float arithmetic |

## Why widen to 64 bits for mul

Without a wide intermediate, `a * b` overflows 32 bits before you shift. **Rejected wrong
reading:** "shift first then multiply to keep it in 32 bits" — loses fraction bits.

## XMM-free requirement

**Rejected wrong reading:** "I'll use `float` and cast at the end." That fails the
integer-asm requirement for your mul/div bodies. Printing may still touch float elsewhere
— focus on the fixed-point routines.

## Contrast with exercise 16 / 63

Floats use IEEE fields and XMM on this ABI; Q16.16 uses scaled integers and GPRs.

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

## Worked constants without float in the hot path

Prefer integer constructions: `1 << 16` for one; `(3 << 16) / 2` for 1.5 when teaching
without float in the q_* bodies. Match harness rounding policy.

**Asm navigation:** `objdump -d` your `q_mul`/`q_div`; confirm integer multiplies/shifts;
reject `mulss`/`addss`/XMM float ops in those bodies.

## Distinctions worth keeping straight

- **Q16.16 vs float** — scale factor versus IEEE fields (exercise 16 contrast).
- **Wide intermediate** — required for correct mul.
- **Integer asm vs XMM** — success criterion for this lab.
- **Rounding/overflow** — real fixed-point issues; harness defines the expected policy.

## Deeper worked navigation (Q16.16 fixed)

- 1.0 = 1<<16.
- mul needs int64 intermediate.
- Asm should be integer, not XMM float.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| 1.5*2.0 | 3.0 in Q16.16 |
| objdump q_* | imul/sar; no mulss |
| harness | ok |

### Ordered navigation moves

1. Encode.
2. mul/div.
3. Asm check.

### Rejected wrong readings (keep beside the artifact)

- use float then cast.
- shift first then mul in 32 bits.
- XMM float ops OK in q_mul.

### Tool-line decoding reminders

- scale 65536
- wide intermediate
- GPR vs XMM

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (68)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - float in q_mul body
- no wide intermediate

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

1. What `int32_t` encoding represents `1.0` in Q16.16?
2. Why does multiplication need a 64-bit intermediate?
3. What asm evidence shows you avoided float ops?
4. How does this contrast with exercise 16?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

Anchor drill: write the exact search string you used in the primary listing, then
quote one real line from your machine with every field labeled. If you cannot label
a field, that field is still unlearned — return to the decoding table above.

## Key takeaways

- Fixed-point stores fractions in scaled integers.
- Q16.16 mul/div are shift + wide integer arithmetic.
- Asm should look integer, not XMM float.
- Useful when you need deterministic scaling without FP units/paths.

## Field-decoding recap for exercise 68

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 16

Now open `TASK.md` and do the practice.
