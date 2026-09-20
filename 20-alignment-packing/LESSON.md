# 20 — Packed vs aligned structs

Natural layout inserts padding so each field sits on its preferred alignment
(lesson 06). `__attribute__((packed))` asks GCC to drop that padding, packing
fields tightly. The win is smaller `sizeof` and stable wire layouts; the cost is
often clumsier loads — split accesses, narrower moves, or extra instructions —
because the CPU (and ABI) prefer aligned addresses. By the end of this lesson
you should compare twin structs (normal vs packed) in both `sizeof` and asm load
shapes.

## What this lesson asks of you

Define the same logical fields twice: ordinary and packed. Write readers through
pointers, print `sizeof`, and diff `.s` offsets / instruction patterns. Optional:
`objdump -h` on the `.o` for alignment notes. `TASK.md` has the checklist.

## What packing changes

Offsets shrink when padding dies. An `int` that used to sit at offset 4 after a
`char` might now sit at offset 1. Asm must then load that `int` from a misaligned
address — sometimes one instruction still works, sometimes GCC emits multiple
smaller loads or helper sequences. Count instructions and note widths; do not
assume every packed field becomes dramatically "slow" on every CPU, but do
expect *different* codegen.

Aligned (natural) layout tends to emit clean `movl offset(%reg), …` forms when
offsets cooperate.

## When people pack

Packed structs appear in binary file formats, protocol headers, and hardware
register maps where bytes on the wire cannot include C's comfort padding. They
are not a general performance tip — often the opposite for in-memory compute.

## Arrays of packed structs

`sizeof` matters doubly when you place packed structs in an array: the stride
between elements shrinks with packing. Code that assumes natural `sizeof` from
a textbook layout will mis-index. Print `sizeof` for both types before trusting
any manual pointer arithmetic in experiments.

## Worked example

**The situation.** `struct Nat { char a; int b; char c; };` and the packed twin
`struct Pack` with the same members. Readers touch every field.

**Step 1 — print sizes.** Packed should be smaller (often 6 vs 12, depending on
tail rules — believe your printer). The rejected wrong reading is: "packed and
natural always share offsets; only section alignment changes." Offsets change.

**Step 2 — compare load offsets in `.s`.** Map each field. The rejected wrong
reading is: "if I still see `movl`, packing failed." Packing can still use a
single move on x86; look at the *offset* and surrounding instructions.

**Step 3 — look for clumsy access.** Split loads, byte helpers, or extra
arithmetic around packed `b` are the smoking gun when present.

**Step 4 — optional `objdump -h`.** Note any align fields on sections / symbols
if useful; do not force a story if headers are unremarkable.

## Distinctions worth keeping straight

- **Natural padding vs packed density** — comfort versus compactness.
- **Offset change vs optional instruction-count cost** — always offsets; cost
  varies.
- **Wire/ABI layouts vs compute structs** — different jobs.
- **Lesson 06 prediction vs lesson 20 opt-in** — packing is explicit.

## Check yourself

1. What attribute opts into packed layout in GNU C?
2. Why might an `int` load look different once its offset becomes 1?
3. Is a larger instruction count for packed access guaranteed on every platform?
4. When is packing justified even if asm looks clumsier?

## Key takeaways

- Packing removes padding; offsets and `sizeof` shrink.
- Asm may show clumsier loads for misaligned fields.
- Use packing for layout contracts, not as a default speed hack.
- Diff natural vs packed twins in both C sizes and `.s`.

## Lookup (not the lesson)

- `man 1 gcc`, `info gcc` — attributes
- `man 1 objdump` — `-d`, `-h`

Now open `TASK.md` and do the practice.
