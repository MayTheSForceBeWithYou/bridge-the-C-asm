# 06 — Struct layout / padding
<!-- concepts: c.p2.memory.sizeof, c.p5.abi.layout -->

A C struct is a ordered bag of fields with alignment rules that insert *padding*
bytes you never name in source. Those bytes still show up as gaps between load
offsets in assembly and as larger `sizeof` values than a naive sum of field
sizes. By the end of this lesson you should predict padding from field order,
confirm it with offsets in `.s`, and see why reordering fields can shrink a
struct.

## What this lesson asks of you

Define at least two layouts with intentional padding (for example `{char; int;
char;}` versus `{int; char; char;}`), read every field through a pointer, and
compare asm offsets to `sizeof` / offsetof-style reasoning. `TASK.md` holds the
lab steps.

## Alignment in one paragraph

Each type has an alignment requirement (an `int` wants a multiple of 4 on this
ABI; a `char` wants 1). The compiler places field `k` at the smallest offset that
respects its alignment given the previous fields. At the end, the struct's
`sizeof` is rounded up so an array of structs keeps every element's first field
aligned. Padding is those skipped bytes — real memory, usually uninitialized
junk unless you zero the object.

## Offsets in assembly

A function that does `p->c`, `p->i`, … becomes loads like `movzbl N(%rdi), …` or
`movl M(%rdi), …` when `p` arrived in `%rdi`. The constants `N` and `M` *are* the
field offsets. Matching them against your mental layout is the whole game.

If two layouts swap field order, the same logical member may sit at different
offsets — and `sizeof` may shrink when packing density improves. That is not the
compiler "optimizing away padding" ad hoc; it is applying the same rules to a
better order.

## offsetof without a library lecture

You can print `(char*)&p->field - (char*)p` safely for a live object to see an
offset experimentally. That C expression is just arithmetic on addresses — the
same numbers you should see as displacements in the `.s`. Use it to double-
check when verbose comments are scarce.

## Worked example

**The situation.** You define `struct A { char a; int b; char c; };` and
`struct B { int b; char a; char c; };`, print `sizeof` for both, and write
`read_a` / `read_b` that touch every field.

**Step 1 — predict offsets on paper.** For `A`, `a` at 0, padding before `b`,
`b` at 4, `c` at 8, then tail padding to a multiple of `int` alignment. For `B`,
tighter packing is possible. The rejected wrong reading is: "`sizeof` is always
the sum of sizeof fields." Padding makes that false.

**Step 2 — confirm with prints.** `sizeof` / computed offsets should match the
prediction. The rejected wrong reading is: "printing sizes changes layout."
Sizes are properties of types, not of `printf`.

**Step 3 — confirm with `.s`.** Field loads' offsets must match. The rejected
wrong reading is: "the first load in the file is always offset 0." Compilers may
reorder loads; identify by *which* field's value is used, with verbose comments
or by distinctive values stored beforehand.

**Step 4 — optional packed preview.** A `__attribute__((packed))` twin (lesson
20) can remove padding at the cost of clumsier accesses. Note it as a contrast,
not the default.

## Distinctions worth keeping straight

- **Field order vs sizeof** — same members, different waste.
- **Padding vs explicit fields** — padding has no name but occupies bytes.
- **Offset in asm vs C member access** — two views of one layout.
- **Natural layout vs packed** — convenience versus wire/compact representation.

## Check yourself

1. Why might `{char; int; char}` be larger than `{int; char; char}`?
2. How do you read a field's offset from AT&T asm that uses `(%rdi)`?
3. Why does the compiler add padding at the *end* of a struct sometimes?
4. If loads appear in a different order than your struct declaration, did the
   layout change?

## Key takeaways

- Alignment inserts padding; `sizeof` includes it.
- Asm load offsets are evidence of layout.
- Reordering fields can change size without changing semantics of named members.
- Packed layouts are an explicit opt-in (see lesson 20).

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, attributes overview in `info gcc` after practice
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
