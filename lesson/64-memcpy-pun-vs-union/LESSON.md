# 64 — memcpy type pun versus union

Exercise 63 used a union to view a `float`'s bits. The C standard's friendly portable
pattern for reinterpreting object representation is often `memcpy` into an integer of
the same size. At `-O2`, GCC typically collapses both to the same load. This lesson
compares the two in source and in asm.

## What this lesson asks of you

Implement `bits_memcpy` and `bits_union` returning the `uint32_t` pattern of a `float`.
Confirm identical results. At `-O2`, inspect asm/disasm and note whether they collapse
to essentially the same load. Record the distinction in notes.

## Why memcpy appears in careful code

`memcpy` between two objects of the same size reinterprets representation without
claiming an aliasing union active. Compilers recognize the idiom and avoid an actual
library call when optimizing. You still write `memcpy` for clarity and conformance
habits; the optimizer erases the ceremony.

Unions remain common in systems code and teaching; knowing both readings matters.

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make O=2` | both pun paths agree on bits |
| `make asm` / `objdump -d` | both collapse toward a 32-bit load |

## Why `memcpy` appears in careful code

Same-size `memcpy` reinterprets representation without claiming an active aliasing union.
Compilers recognize the idiom and erase the call at `-O2`.

**Rejected wrong reading:** "`memcpy` must be slower because it is a function call."

## Asm recognition

Look for a simple move/load of 32 bits. Cosmetic register-name differences do not change
the bit story. **Rejected wrong reading:** different register names mean different
semantics.

## Union vs memcpy (teaching vs policy)

Unions remain a clear teaching pun; `memcpy` is the standard-friendly habit. Both must
return identical bits for the same float.

## Worked example

**The situation.** Both functions return `0x3f800000` for `1.0f`; `-O2` asm shows a
simple move/load for each; `ok`.

**Step 1 — implement both.** Same input, same output type. The rejected wrong reading
is: "memcpy must be slower because it is a function call." At `-O2` it is usually
intrinsic/inlined into a load.

**Step 2 — compare asm.** `make O=2 asm disasm`. The rejected wrong reading is: "if asm
differs in register names, the methods are semantically different." Look for the load of
32 bits; cosmetic asm differences do not change the bit story.

**Step 3 — notes.** Union vs memcpy: teaching pun vs standard-friendly reinterpret.

## Worked equivalence checklist

| Check | Pass looks like |
| ----- | --------------- |
| `bits_memcpy(1.0f)` | `0x3f800000` |
| `bits_union(1.0f)` | same |
| `-O2` asm | both ≈ 32-bit load/move |

Write one note sentence: teaching pun (union) vs standard-friendly reinterpret
(`memcpy`).

## Distinctions worth keeping straight

- **Source-level rules vs optimized bytes** — legality vs what `-O2` emits.
- **Idiom recognition** — memcpy-of-4-bytes → load.
- **Identical bits** — required success; asm "essentially equivalent" is the lesson.
- **Alias analysis** — broader topic; this lab stays on the 32-bit float pattern.

## Deeper worked navigation (memcpy vs union)

- Same bits required.
- O2 memcpy → load.
- Teaching pun vs standard habit.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| both returns | identical |
| O2 asm | essentially load |
| notes | distinction sentence |

### Ordered navigation moves

1. Implement both.
2. Compare results.
3. Compare asm.

### Rejected wrong readings (keep beside the artifact)

- memcpy must be slower.
- register-name diffs ⇒ different semantics.

### Tool-line decoding reminders

- idiom recognition
- -O2 disasm
- aliasing note

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (64)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - memcpy must be slow
- cosmetic asm diffs as semantic

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

1. Why do careful codebases often prefer `memcpy` for punning?
2. What should `-O2` typically do to a 4-byte `memcpy` pun?
3. Must union and memcpy return the same bits for the same float?
4. What artifact proves they collapsed?

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

- `memcpy` is the standard-friendly reinterpret idiom.
- Unions remain a clear teaching pun.
- Optimizers usually make them equally cheap.
- Always check results *and* asm before claiming equivalence.

## Field-decoding recap for exercise 64

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 3 memcpy`, `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
