# 17 — Linked list pointer chasing

A linked list turns field offsets and pointer loads into a loop: load `val`, load
`next`, then chase `next` until you see NULL. In assembly that is a small machine
you can read cold — offset `0` versus `8` on a typical `struct Node { int val;
struct Node *next; }` under LP64 alignment, plus a compare-and-branch against
zero. By the end of this lesson you should identify those loads at `-O0` and
notice how `-O2` schedules or tightens the same chase.

## What this lesson asks of you

Define `struct Node`, implement `list_sum`, build a short stack-allocated list in
`main`, and compare asm at `-O0` and `-O2`. `TASK.md` is the practice.

## What `make` is naming for you

| You type | Open | Search |
| -------- | ---- | ------ |
| `make asm` | `prog.s` | `list_sum:`, loads with `(%reg)` / `N(%reg)` |
| Copy + `make O=2 asm` | `prog-O2.s` | same function, tighter loop? |
| `make disasm` | `prog.lst` | `<list_sum>:` |

## Layout on the wire (in memory)

With an `int` at offset 0 and a pointer following, padding may appear after
`val` so `next` sits on an 8-byte boundary — often making `next` land at offset
8 (`sizeof(int)` + 4 bytes pad). Your `.s` load offsets settle the question; do
not argue from hope. Lesson 06 already trained that instinct; this lesson applies
it inside a loop.

Paper prediction for `struct Node { int val; struct Node *next; };`:

| Field | Offset | Width of access |
| ----- | ------ | --------------- |
| `val` | 0 | `movl` (4 bytes) |
| *(pad)* | 4..7 | not accessed by name |
| `next` | 8 | `movq` (8 bytes) |

### How to decode the chase loads

A typical `-O0` loop body shape:

```text
.L_loop:
        movq    -8(%rbp), %rax       # head / cur pointer
        movl    (%rax), %eax         # cur->val at offset 0
        addl    %eax, -12(%rbp)      # s += val
        movq    -8(%rbp), %rax
        movq    8(%rax), %rax        # cur->next at offset 8
        movq    %rax, -8(%rbp)       # cur = next
        movq    -8(%rbp), %rax
        testq   %rax, %rax           # NULL?
        jne     .L_loop
```

| Instruction | Recognition rule |
| ----------- | ---------------- |
| `movl (%rax), %eax` | 32-bit load at offset 0 → `val` |
| `movq 8(%rax), %rax` | 64-bit load at offset 8 → `next` |
| `testq %rax,%rax` / `cmpq $0,…` + `jne`/`je` | NULL check — ordinary flags, not a magic opcode |
| Loop label + back-edge | chase continues while non-NULL |

### Navigation

1. Search `list_sum:`.
2. Skip prologue; find the loop label (`.L…`).
3. Classify every memory operand as `val`, `next`, spill slot (`-N(%rbp)`), or
   other.
4. Find the NULL test and name the exit label.

**Rejected wrong reading:** "the first memory operand in the function is always
`val`." Prologue spills or reordered loads can appear first; identify by width
and use (add into sum vs move into head pointer).

**Rejected wrong reading:** "NULL checks use a special `isnull` opcode." It is
ordinary compare/test + jump.

## Pointer chasing vs array walks

Each node touch is at least one load for data and one for the successor (unless
optimization fuses or hoists). Unlike an array walk, the next address is
*data-dependent* — you cannot compute all addresses from an index alone. That is
why list asm looks like "load, load, branch" rather than scaled-index addressing
from a fixed base (lesson 07).

| Structure | Address of "next element" |
| --------- | ------------------------- |
| `int a[n]` | `base + 4*i` — computable from index |
| linked list | loaded from `cur->next` — data-dependent |

## Stack-allocated nodes

Building the list with automatic `struct Node` objects in `main` keeps the
exercise free of `malloc`, but the *links* are still ordinary pointers. The asm
for `list_sum` does not care whether nodes live on the stack or the heap — only
that `next` chains eventually reach NULL.

In `main`'s `.s` you may see stack slots for each node and `leaq -N(%rbp), %rax`
stores into `8(%reg)` to link them — that is list *construction*, separate from
`list_sum`'s chase.

## What `-O2` may change

Expect possible:

- Fewer spills; `cur` living in a register across the loop.
- Reordered `val`/`next` loads (still correct if no aliasing issues).
- Tighter compare/branch structure.
- Unchanged offsets `0` and `8` for the fields — layout does not float with `-O`.

**Rejected wrong reading:** "if `-O2` reorders the `val` and `next` loads, the C
code is wrong." Scheduling ≠ semantics failure.

## Worked example

**The situation.** Three stack nodes linked head→…→NULL; `list_sum` returns the
sum of `val` fields.

**Step 1 — find loads in `list_sum` at `-O0`.** Match offsets to `val` vs `next`
using sizeof reasoning and verbose comments.

**Step 2 — find the NULL test.** Note which register is tested and where the
exit label sits.

**Step 3 — `-O2`.** See whether loads reorder, whether the loop tightens, or
whether induction looks different while semantics remain.

**Step 4 — contrast with arrays.** One sentence: list addresses come from memory;
array addresses come from base+scale×index.


## Confirming padding with `sizeof` / offsetof

Print `sizeof(struct Node)` and `(char*)&node.next - (char*)&node`. If you get
`16` and `8`, the asm `movq 8(%rax), …` story is confirmed before you open the
listing. If you ever invent a weird struct (pointer first, etc.), redo the paper
offsets — do not reuse 0/8 blindly.


## Empty list and single node

Check the zero-trip path: if `head == NULL`, `list_sum` should return 0 without
loading `val`. In asm, that is an initial `test`/`cmp` before the loop body. A
single-node list should load `val` once, load `next` (NULL), then exit. Trace
those cases on paper against your graph even if you only run a three-node test.


## Width checklist

| Access | Expected mnemonic family | Expected offset (typical Node) |
| ------ | ------------------------ | ------------------------------ |
| `val` | `movl` / `mov` 32-bit | 0 |
| `next` | `movq` / `mov` 64-bit | 8 |

If your offsets differ, your struct layout differs — update the table from
evidence, do not force 0/8.

## Distinctions worth keeping straight

- **`val` offset vs `next` offset** — data versus link (`movl` vs `movq` width).
- **Data-dependent chase vs indexed array walk** — latency shapes differ.
- **NULL as zero pointer vs C `NULL` macro** — same machine test.
- **Construction in `main` vs summation in `list_sum`** — different functions.
- **`-O0` obvious loop vs `-O2` scheduling** — same chase, different texture.

## Check yourself

1. How do you confirm the offset of `next` from asm alone?
2. What instruction pattern typically ends a list walk?
3. Why can list summation not use a simple `(%base,%idx,4)` for node pointers?
4. If `-O2` reorders the `val` and `next` loads, is the C code wrong?
5. Why is `movl (%rax),%eax` more likely `val` than `next` on this struct?
6. What search string finds the chase without reading all of `main`?

## Key takeaways

- List walking is load/`next`/branch, driven by field offsets.
- NULL exits are ordinary zero tests.
- Width + displacement identify `val` vs `next` (`0`/`movl` vs `8`/`movq` often).
- `-O2` may reshape scheduling without changing the abstract chase.
- Read offsets from `.s`; apply struct-layout skills inside loops.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
