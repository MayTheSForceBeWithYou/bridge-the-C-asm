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

## Layout on the wire (in memory)

With an `int` at offset 0 and a pointer following, padding may appear after
`val` so `next` sits on an 8-byte boundary — often making `next` land at offset
8. Your `.s` load offsets settle the question; do not argue from hope. Lesson 06
already trained that instinct; this lesson applies it inside a loop.

NULL testing is usually `cmp $0, reg` / `test reg, reg` followed by a conditional
jump out of the loop. The pointer from `next` becomes the new base for the
following iteration's loads.

## Pointer chasing costs

Each node touch is at least one load for data and one for the successor (unless
optimization fuses or hoists). Unlike an array walk, the next address is *data-
dependent* — you cannot compute all addresses from an index alone. That is why
list asm looks like "load, load, branch" rather than scaled-index addressing
from a fixed base (lesson 07).

## Stack-allocated nodes

Building the list with automatic `struct Node` objects in `main` keeps the
exercise free of `malloc`, but the *links* are still ordinary pointers. The asm
for `list_sum` does not care whether nodes live on the stack or the heap — only
that `next` chains eventually reach NULL.

## Worked example

**The situation.** Three stack nodes linked head→…→NULL; `list_sum` returns the
sum of `val` fields.

**Step 1 — find loads in `list_sum` at `-O0`.** Match offsets to `val` vs `next`
using sizeof reasoning and verbose comments. The rejected wrong reading is:
"the first memory operand in the function is always `val`." Prologue spills or
reordered loads can appear first; identify by width and use (add into sum vs
move into head pointer).

**Step 2 — find the NULL test.** Note which register is tested and where the
exit label sits. The rejected wrong reading is: "NULL checks use a special
`isnull` opcode." It is ordinary compare/test + jump.

**Step 3 — `-O2`.** See whether loads reorder, whether the loop tightens, or
whether induction looks different while semantics remain. Record observations.

**Step 4 — contrast with arrays.** One sentence: list addresses come from memory;
array addresses come from base+scale×index.

## Distinctions worth keeping straight

- **`val` offset vs `next` offset** — data versus link.
- **Data-dependent chase vs indexed array walk** — latency shapes differ.
- **NULL as zero pointer vs C `NULL` macro** — same machine test.
- **`-O0` obvious loop vs `-O2` scheduling** — same chase, different texture.

## Check yourself

1. How do you confirm the offset of `next` from asm alone?
2. What instruction pattern typically ends a list walk?
3. Why can list summation not use a simple `(%base,%idx,4)` for node pointers?
4. If `-O2` reorders the `val` and `next` loads, is the C code wrong?

## Key takeaways

- List walking is load/`next`/branch, driven by field offsets.
- NULL exits are ordinary zero tests.
- `-O2` may reshape scheduling without changing the abstract chase.
- Read offsets from `.s`; apply struct-layout skills inside loops.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
