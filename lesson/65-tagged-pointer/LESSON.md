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

## Mechanism formulas

```text
tagged = (uintptr_t)p | (tag & 7)
tag    = tagged & 7
ptr    = (void *)(tagged & ~(uintptr_t)7)
```

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make` | round-trip pointer + tag; `ok` |
| `make asm` | `and`/`or` with 7 / `~7` |

## Alignment precondition

≥8-byte alignment ⇒ low 3 bits zero ⇒ free for a 3-bit tag. **Rejected wrong reading:**
hardware ignores low bits on dereference — it does not; untag first.

**Rejected wrong reading:** "shift right by 3 to recover the pointer." That destroys the
address; AND with `~7`.

## Asm navigation

Search your function labels; confirm masking immediates. Tagging should look cheap —
integer `and`/`or`, not a library call.

## Worked example

**The situation.** You tag a `malloc`ed pointer with tag `5`, recover both, `ok`; asm
shows `and` with immediate 7 / mask `~7`.

**Step 1 — tag.** OR in the low bits. The rejected wrong reading is: "I can dereference
the tagged pointer directly because hardware ignores low bits." Hardware does not; you
will misalign or fault.

**Step 2 — untag / get_tag.** Mask correctly. The rejected wrong reading is: "shift
right by 3 to get the pointer." That destroys the address; you want AND with `~7`.

**Step 3 — asm.** Confirm `and`/`or` (or LEA tricks); tie to exercise bit ops.

## Worked round-trip

Tag a `malloc`ed pointer with tag `5`, recover pointer and tag, `ok`. In asm, find
`and`/`or` with 7 / `~7`.

**Never** dereference the tagged value raw. ABI alignment (≥8) is the precondition —
document it; do not tag arbitrary function pointers without alignment guarantees.

## Distinctions worth keeping straight

- **Tag bits vs address bits** — disjoint only under alignment.
- **3-bit tag space** — values 0..7; mask with 7.
- **Untag before use** — mandatory.
- **ABI alignment assumption** — document it; do not tag arbitrary function pointers
  without checking alignment guarantees.

## Deeper worked navigation (tagged pointer)

- Low 3 bits free if align≥8.
- OR tag; AND to extract/clear.
- Untag before dereference.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| round-trip | ptr+tag ok |
| asm | and/or masks |
| align assumption | documented |

### Ordered navigation moves

1. tag.
2. untag/get.
3. asm check.

### Rejected wrong readings (keep beside the artifact)

- deref tagged raw.
- shift right 3 to get pointer.
- tag any function pointer blindly.

### Tool-line decoding reminders

- mask 7 / ~7
- alignment precondition
- cheap integer ops

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (65)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - deref tagged pointer
- shift to untag
- tag without alignment story

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

1. Why do low bits of an 8-byte-aligned pointer start as zero?
2. How do you recover the pointer from a tagged value?
3. What goes wrong if you load through a still-tagged pointer?
4. What asm ops should appear for tag/untag?

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

- Alignment frees low bits for tags.
- OR to set, AND to extract or clear.
- Always untag before dereference.
- Asm makes the masking visible and cheap.

## Field-decoding recap for exercise 65

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
