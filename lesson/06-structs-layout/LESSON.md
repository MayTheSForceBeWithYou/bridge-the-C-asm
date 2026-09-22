# 06 — Struct layout / padding

A C struct is an ordered bag of fields with alignment rules that insert *padding*
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

## What `make` is naming for you

| You type | Open | Why |
| -------- | ---- | --- |
| `make` / run the binary | stdout | `sizeof` / printed offsets |
| `make asm` | `prog.s` | field loads' displacements |
| `make disasm` | `prog.lst` | same loads after link; search `<read_a>:` |

## Alignment in one paragraph

Each type has an alignment requirement (an `int` wants a multiple of 4 on this
ABI; a `char` wants 1). The compiler places field `k` at the smallest offset that
respects its alignment given the previous fields. At the end, the struct's
`sizeof` is rounded up so an array of structs keeps every element's first field
aligned. Padding is those skipped bytes — real memory, usually uninitialized
junk unless you zero the object.

### Paper prediction for two layouts

For `struct A { char a; int b; char c; };` on this ABI:

| Field | Offset | Why |
| ----- | ------ | --- |
| `a` | 0 | `char` align 1 |
| *(pad)* | 1..3 | so `int b` can sit at multiple of 4 |
| `b` | 4 | `int` |
| `c` | 8 | next free byte |
| *(tail pad)* | 9..11 | so `sizeof` is multiple of max align (4) → often 12 |

For `struct B { int b; char a; char c; };`:

| Field | Offset | Why |
| ----- | ------ | --- |
| `b` | 0 | |
| `a` | 4 | |
| `c` | 5 | |
| *(tail pad)* | 6..7 | round to multiple of 4 → often 8 |

**Rejected wrong reading:** "`sizeof` is always the sum of sizeof fields." Padding
makes that false — `1+4+1` is 6, but `sizeof(struct A)` is often 12.

## Offsets in assembly — how to decode a field load

A function that does `p->c`, `p->i`, … becomes loads when `p` arrived in `%rdi`.
Real `-O0` shapes for `read_a` / `read_b`:

```text
# struct A { char a; int b; char c; };  — p in %rax after reload
        movzbl  (%rax), %eax      # p->a at offset 0
        …
        movl    4(%rax), %eax     # p->b at offset 4
        …
        movzbl  8(%rax), %eax     # p->c at offset 8

# struct B { int b; char a; char c; };
        movl    (%rax), %eax      # p->b at offset 0
        movzbl  4(%rax), %eax     # p->a at offset 4
        movzbl  5(%rax), %eax     # p->c at offset 5
```

| Asm piece | Meaning |
| --------- | ------- |
| `(%rax)` or `0(%rax)` | offset 0 from the struct base |
| `4(%rax)` | offset 4 — here, `int b` in `A` or `char a` in `B` |
| `movzbl` vs `movl` | byte load zero-extended vs 32-bit load — matches `char` vs `int` |
| Verbose comment `p_8(D)->b` | GCC naming the field (trust, then verify) |

### Navigation move

1. Search `read_a:` in `prog.s`.
2. Skip prologue; find loads that use the pointer (often after `movq %rdi, -8(%rbp)`
   and a reload into `%rax`).
3. Record every displacement `N` in `N(%reg)` together with the mnemonic width.
4. Repeat for `read_b:` and compare the displacement tables — same member names,
   different numbers.

**Rejected wrong reading:** "the first load in the file is always offset 0."
Compilers may reorder loads; identify by *which* field's value is used, with
verbose comments or by distinctive values stored beforehand.

**Rejected wrong reading:** "printing sizes changes layout." Sizes are properties
of types, not of `printf`.

## offsetof without a library lecture

You can print `(char*)&p->field - (char*)p` safely for a live object to see an
offset experimentally. That C expression is just arithmetic on addresses — the
same numbers you should see as displacements in the `.s`. Use it to double-check
when verbose comments are scarce.

## Packed preview (lesson 20)

A `__attribute__((packed))` twin can remove padding at the cost of clumsier
accesses. Note it as a contrast, not the default — lesson 20 owns that deep dive.

## Worked example

**The situation.** You define `struct A { char a; int b; char c; };` and
`struct B { int b; char a; char c; };`, print `sizeof` for both, and write
`read_a` / `read_b` that touch every field.

**Step 1 — predict offsets on paper** using the tables above.

**Step 2 — confirm with prints.** `sizeof` / computed offsets should match.

**Step 3 — confirm with `.s`.** Field loads' offsets must match. For `A`, expect
displacements 0 / 4 / 8; for `B`, 0 / 4 / 5 (with possible reordering of *which
load comes first*).

**Step 4 — optional packed twin.** If you add one, note smaller `sizeof` and
changed displacements — then leave the messy access story for lesson 20.


## `sizeof` vs sum — worked numbers

For `struct A { char a; int b; char c; }`:

- Sum of field sizes: `1+4+1 = 6`
- With alignment padding: often `12`
- Waste: 6 bytes you never named

For `struct B { int b; char a; char c; }`:

- Sum still 6
- Packed denser: often `sizeof` 8 (2 bytes tail pad for align-4)
- Same members, less waste — **reordering paid for itself**

Print both sizes in the program before arguing with the `.s`. If asm offsets
disagree with your paper table, fix the paper table — the displacements are the
ground truth for that build.

### Objdump field load

```text
    1162:       8b 40 04                mov    0x4(%rax),%eax
```

| Token | Meaning |
| ----- | ------- |
| `1162:` | instruction address |
| `0x4(%rax)` | field at offset 4 from struct base in `%rax` |
| `mov` / `movl` | 32-bit load → likely an `int` field |

## Distinctions worth keeping straight

- **Field order vs sizeof** — same members, different waste.
- **Padding vs explicit fields** — padding has no name but occupies bytes.
- **Offset in asm vs C member access** — two views of one layout.
- **Load order vs declaration order** — compiler may rearrange instructions.
- **Natural layout vs packed** — convenience versus wire/compact representation.

## Check yourself

1. Why might `{char; int; char}` be larger than `{int; char; char}`?
2. How do you read a field's offset from AT&T asm that uses `(%rdi)` / `N(%rax)`?
3. Why does the compiler add padding at the *end* of a struct sometimes?
4. If loads appear in a different order than your struct declaration, did the
   layout change?
5. What does `movzbl 5(%rax), %eax` tell you about both offset and field width?
6. What search string takes you to your reader without scrolling CRT?

## Key takeaways

- Alignment inserts padding; `sizeof` includes it.
- Asm load offsets are evidence of layout — decode `N(%reg)` + mnemonic width.
- Reordering fields can change size without changing semantics of named members.
- Packed layouts are an explicit opt-in (see lesson 20).

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, attributes overview in `info gcc` after practice
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
