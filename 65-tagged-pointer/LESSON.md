# 65 — Stashing tags in aligned pointers

Heap pointers from `malloc` on this ABI are aligned to at least 8 bytes (often more).
The low 3 bits of such a pointer are therefore zero and can store a small *tag* — a
type discriminant or flags — if you mask them away before dereferencing. This lesson
builds `tag_ptr` / `untag_ptr` / `get_tag` and finds the `and`/`or` in asm.

## What this lesson asks of you

Assume ≥8-byte alignment. Implement tagging with a 3-bit tag, round-trip pointer and
tag, print `ok`, and inspect masking ops in `make asm` output.

## Mechanism

- `tagged = (uintptr_t)p | (tag & 7)`
- `tag = tagged & 7`
- `ptr = (void *)(tagged & ~(uintptr_t)7)`

Never dereference the tagged value without clearing the low bits. Alignment is the
precondition that makes those bits free; if a pointer were only 1-byte aligned, tagging
would destroy address information.

## Worked example

**The situation.** You tag a `malloc`ed pointer with tag `5`, recover both, `ok`; asm
shows `and` with immediate 7 / mask `~7`.

**Step 1 — tag.** OR in the low bits. The rejected wrong reading is: "I can dereference
the tagged pointer directly because hardware ignores low bits." Hardware does not; you
will misalign or fault.

**Step 2 — untag / get_tag.** Mask correctly. The rejected wrong reading is: "shift
right by 3 to get the pointer." That destroys the address; you want AND with `~7`.

**Step 3 — asm.** Confirm `and`/`or` (or LEA tricks); tie to exercise bit ops.

## Distinctions worth keeping straight

- **Tag bits vs address bits** — disjoint only under alignment.
- **3-bit tag space** — values 0..7; mask with 7.
- **Untag before use** — mandatory.
- **ABI alignment assumption** — document it; do not tag arbitrary function pointers
  without checking alignment guarantees.

## Check yourself

1. Why do low bits of an 8-byte-aligned pointer start as zero?
2. How do you recover the pointer from a tagged value?
3. What goes wrong if you load through a still-tagged pointer?
4. What asm ops should appear for tag/untag?

## Key takeaways

- Alignment frees low bits for tags.
- OR to set, AND to extract or clear.
- Always untag before dereference.
- Asm makes the masking visible and cheap.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
