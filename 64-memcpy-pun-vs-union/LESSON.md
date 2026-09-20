# 64 — memcpy type pun versus union
<!-- concepts: c.p5.bits.pun -->

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

## Distinctions worth keeping straight

- **Source-level rules vs optimized bytes** — legality vs what `-O2` emits.
- **Idiom recognition** — memcpy-of-4-bytes → load.
- **Identical bits** — required success; asm "essentially equivalent" is the lesson.
- **Alias analysis** — broader topic; this lab stays on the 32-bit float pattern.

## Check yourself

1. Why do careful codebases often prefer `memcpy` for punning?
2. What should `-O2` typically do to a 4-byte `memcpy` pun?
3. Must union and memcpy return the same bits for the same float?
4. What artifact proves they collapsed?

## Key takeaways

- `memcpy` is the standard-friendly reinterpret idiom.
- Unions remain a clear teaching pun.
- Optimizers usually make them equally cheap.
- Always check results *and* asm before claiming equivalence.

## Lookup (not the lesson)

- `man 3 memcpy`, `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
