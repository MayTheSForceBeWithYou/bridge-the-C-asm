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

## What `make` is naming for you

| You type | Open | Compare |
| -------- | ---- | ------- |
| Run binary | stdout | `sizeof(Nat)` vs `sizeof(Pack)` |
| `make asm` | `prog.s` | `read_n:` vs `read_p:` displacements |
| `objdump -h prog.o` (optional) | section/align hints | do not force a story |

## What packing changes — predict then measure

Offsets shrink when padding dies. An `int` that used to sit at offset 4 after a
`char` might now sit at offset 1.

| Layout | `a` | `b` | `c` | Typical `sizeof` |
| ------ | --- | --- | --- | ---------------- |
| `struct Nat { char a; int b; char c; }` | 0 | 4 | 8 | 12 |
| `struct Pack` packed same members | 0 | 1 | 5 | 6 |

### Decode loads for each twin

Natural reader (lesson 06 shape):

```text
        movzbl  (%rax), %eax      # a @ 0
        movl    4(%rax), %eax     # b @ 4
        movzbl  8(%rax), %eax     # c @ 8
```

Packed reader — real GCC often still emits a single misaligned `movl`:

```text
        movzbl  (%rax), %eax      # a @ 0
        movl    1(%rax), %eax     # b @ 1  (misaligned int load)
        movzbl  5(%rax), %eax     # c @ 5
```

| What changed | What may look the same |
| ------------ | ---------------------- |
| Displacements (4→1, 8→5) | Still `movl` for `b` on x86 |
| `sizeof` | Mnemonic family |

**Rejected wrong reading:** "packed and natural always share offsets; only
section alignment changes." Offsets change — that is the point.

**Rejected wrong reading:** "if I still see `movl`, packing failed." Packing can
still use a single move on x86; look at the *offset* and surrounding
instructions. On other arches or older compilers you might see split byte loads
— if you do, that is the "clumsy" story; if you do not, say so honestly.

### Navigation

1. Print both `sizeof` values; write them down before opening asm.
2. Search `read_n:` — record displacements for `a`/`b`/`c`.
3. Search `read_p:` — record displacements; highlight any that are not naturally
   aligned for an `int` (offset `% 4 != 0`).
4. Optional: look for extra instructions around packed `b` (helpers, multiple
   narrower loads). Note absence as well as presence.

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

**Step 1 — print sizes.** Packed should be smaller (often 6 vs 12 — believe your
printer).

**Step 2 — compare load offsets in `.s`.** Map each field.

**Step 3 — look for clumsy access.** Split loads, byte helpers, or extra
arithmetic around packed `b` are the smoking gun *when present*.

**Step 4 — optional `objdump -h`.** Note any align fields on sections / symbols
if useful; do not force a story if headers are unremarkable.


## Misalignment arithmetic

Offset `1` for an `int` means the address is `base+1`, which is not divisible by
4. x86 commonly allows the misaligned `movl` (possibly slower); other ISAs may
trap or require multiple loads. Your job on this track: *observe* GCC's choice
and record offsets. Do not claim "packing always doubles instruction count" if
your listing still shows one `movl`.


## Makefile / print workflow

```bash
make            # run once — print sizeof both types
make asm        # compare read_n vs read_p displacements
```

Write a tiny table in your notes before closing the files:

```text
        offset a   offset b   offset c   sizeof
Nat     ______     ______     ______     ______
Pack    ______     ______     ______     ______
```

Fill offsets from asm; fill sizeof from the program. They must agree.

### When packed loads split

If you ever see multiple byte loads ORBed/shifted into an `int` for packed `b`,
that is the clumsy path. Annotate it. If you do not see it on this box, write
"single movl at offset 1" — that is still a successful observation.


## Rejected wrong reading (wire formats)

"**Packed structs are always faster because they use less memory.**" Smaller can
mean denser cache use, but misaligned loads can cost more — and correctness for
on-wire layouts is the usual reason to pack, not a blanket speed rule.


## Tie-back to lesson 06

Everything you learned about predicting offsets still applies — packing is the
opt-in that *disables* the padding step. If lesson 06's paper tables were fuzzy,
revisit them before trusting packed displacements.

## Distinctions worth keeping straight

- **Natural padding vs packed density** — comfort versus compactness.
- **Offset change vs optional instruction-count cost** — always offsets; cost
  varies by ISA/compiler.
- **Wire/ABI layouts vs compute structs** — different jobs.
- **Lesson 06 prediction vs lesson 20 opt-in** — packing is explicit.
- **`movl` surviving vs packing "failing"** — check the displacement.

## Check yourself

1. What attribute opts into packed layout in GNU C?
2. Why might an `int` load look different once its offset becomes 1?
3. Is a larger instruction count for packed access guaranteed on every platform?
4. When is packing justified even if asm looks clumsier?
5. If `sizeof(Pack)` is 6, what is the stride between `Pack` array elements?
6. Which displacements prove packing in `read_p` even when mnemonics match
   `read_n`?

## Key takeaways

- Packing removes padding; offsets and `sizeof` shrink.
- Asm may show clumsier loads for misaligned fields — or the same mnemonic at a
  weird offset; read the displacement.
- Use packing for layout contracts, not as a default speed hack.
- Diff natural vs packed twins in both C sizes and `.s`.

## Lookup (not the lesson)

- `man 1 gcc`, `info gcc` — attributes
- `man 1 objdump` — `-d`, `-h`

Now open `TASK.md` and do the practice.
