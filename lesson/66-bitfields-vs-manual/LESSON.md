# 66 — C bitfields versus shift/mask packing

C bitfields let you declare members like `unsigned x : 5`. The compiler chooses layout
details (allocation unit, order) in implementation-defined ways. Manual packing with
shifts and masks on a `uint32_t` makes the layout explicit in your code. This lesson
compares `sizeof`, get/set APIs, and generated asm — extending exercise 20's alignment
intuition into bit-level packing.

## What this lesson asks of you

Define a bitfield struct and a manual pack API for the same logical fields. Print
`sizeof`, implement getters/setters for both, compare asm, achieve the same logical
values, and print `ok`.

## Why manual packing exists

Wire protocols, instruction encodings, and cross-language ABIs often need *exact* bit
positions. Bitfields are convenient but can surprise you across compilers/endianness.
Shifts (`(w >> k) & mask`, `w = (w & ~mask) | ((v & mask) << k)`) document the layout in
arithmetic you control.

`sizeof` a bitfield struct may be larger than you naively sum due to the container type
and padding rules — measure it.

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make` | both APIs agree on logical fields; print `sizeof`; `ok` |
| `make asm` | compare bitfield lowering vs explicit `and`/`or`/`shl` |

## Why manual packing exists

Wire protocols and ABIs need *exact* bit positions. Bitfield layout (order, container) is
implementation-defined — verify on *this* GCC.

**Rejected wrong reading:** bitfield member order in memory always matches declaration
order portably.

## `sizeof` recognition

Containers round up. **Rejected wrong reading:** "`sizeof` must equal total bit width /
8." Measure it.

## Manual set/get shape

```text
get: (w >> k) & mask
set: w = (w & ~mask) | ((v & mask) << k)
```

Expect similar `and`/`or`/`shl` after bitfields lower — compare clarity and control.

## Worked example

**The situation.** Both APIs agree on field values; notes record sizeof and whether
bitfield asm looks cleaner or murkier than manual masks.

**Step 1 — define both layouts.** Same widths in intent. The rejected wrong reading is:
"bitfield member order in memory always matches declaration order portably." Order and
packing are implementation-defined; verify on *this* GCC.

**Step 2 — get/set.** Exercise both paths. The rejected wrong reading is: "sizeof must
equal total bit width / 8." Containers round up; measure.

**Step 3 — asm compare.** Manual code often shows explicit `and`/`or`/`shl`; bitfields
may too, after lowering.

## Worked comparison notes

Record `sizeof` both layouts. Implement get/set for each. Confirm identical logical
values. In asm, manual code usually shows explicit `and`/`or`/`shl`; bitfields often
lower similarly — compare clarity and portability, not mythical magic.

Related discipline: exercise 20's struct padding.

## Distinctions worth keeping straight

- **Logical fields vs physical layout guarantees** — bitfields weaker portably.
- **Manual mask constants vs : widths** — explicit vs declarative.
- **Exercise 20 packing** — struct padding vs bit packing; related discipline.
- **Readability vs control** — choose per problem.

## Deeper worked navigation (bitfields vs manual)

- Bitfield layout is impl-defined.
- Manual masks document positions.
- Measure sizeof.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| sizeof | printed |
| get/set both | same logical values |
| asm | and/or/shl shapes |

### Ordered navigation moves

1. Define both.
2. Exercise APIs.
3. Compare asm+sizeof.

### Rejected wrong readings (keep beside the artifact)

- bitfield order always portable.
- sizeof == bitwidth/8.
- manual always slower.

### Tool-line decoding reminders

- mask constants
- impl-defined packing
- ex20 kinship

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (66)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - portable bitfield order assumed
- sizeof == bits/8

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

1. Why might two compilers disagree on bitfield layout?
2. What does manual packing buy you for a wire format?
3. Why print `sizeof` in this lab?
4. What asm shapes do you expect for manual set/get?

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

- Bitfields are convenient and implementation-defined in layout details.
- Shift/mask packing makes positions explicit.
- Measure sizeof; do not assume.
- Same logical values are the functional bar; asm shows the cost/clarity tradeoff.

## Field-decoding recap for exercise 66

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 20

Now open `TASK.md` and do the practice.
