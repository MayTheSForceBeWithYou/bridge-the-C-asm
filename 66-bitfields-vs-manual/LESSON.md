# 66 — C bitfields versus shift/mask packing
<!-- concepts: c.p5.bits.fields -->

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

## Distinctions worth keeping straight

- **Logical fields vs physical layout guarantees** — bitfields weaker portably.
- **Manual mask constants vs : widths** — explicit vs declarative.
- **Exercise 20 packing** — struct padding vs bit packing; related discipline.
- **Readability vs control** — choose per problem.

## Check yourself

1. Why might two compilers disagree on bitfield layout?
2. What does manual packing buy you for a wire format?
3. Why print `sizeof` in this lab?
4. What asm shapes do you expect for manual set/get?

## Key takeaways

- Bitfields are convenient and implementation-defined in layout details.
- Shift/mask packing makes positions explicit.
- Measure sizeof; do not assume.
- Same logical values are the functional bar; asm shows the cost/clarity tradeoff.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 20

Now open `TASK.md` and do the practice.
