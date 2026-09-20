# 07 — Arrays vs pointers

In C source, `a[i]` and `*(p + i)` are closely related, and for a parameter
declared `const int *a` the "array" vs "pointer" spellings in a loop are often
the same idea. What matters in this lesson is whether GCC emits the *same*
instructions for index style versus pointer-increment style — at `-O0` and at
`-O2`. By the end you should spot scaled addressing for `int` (stride 4) and say
whether your two functions converged under optimization.

## What this lesson asks of you

Implement `sum_idx` and `sum_ptr` that sum `n` ints using indexing versus
`*p++`-style traversal. Diff their asm at two optimization levels. `TASK.md` is
the checklist.

## What indexing means in asm

Accessing an `int` element usually multiplies the index by 4 (the element size)
and adds that to the base address. That may appear as:

- Explicit `imul` / shifts + `add`, or
- A single addressing mode such as `(%reg,%reg,4)` or an `leaq` with scale.

Seeing the scale factor `4` is seeing the type's size become machine reality.
Verbose asm helps tie the address computation back to `a[i]`.

## Pointer increment style

Advancing a pointer by one `int` adds 4 to the address. At `-O0`, that often
shows as a distinct add to a pointer local each iteration, with a load through
it. It can look *longer* than indexing even when it is semantically equivalent,
because `-O0` is not trying to impress you with elegance.

## Optimization may erase the difference

At `-O2`, both loops may become the same strength-reduced form: a running
address or a scaled index with similar load/accumulate patterns. They might also
vectorize later in the track; here you only need to notice convergence or
remaining cosmetic differences.

The rejected wrong reading is: "if the asm differs, the C semantics differ."
Equivalent C can have different instruction textures, especially at `-O0`.

## Worked example

**The situation.** Both functions sum the same array contents; `main` checks
they agree.

**Step 1 — `-O0` diff.** Place the two function bodies side by side. Note
whether instruction sequences are identical or only equivalent. The rejected
wrong reading is: "pointer style is always faster because it shows fewer
multiplies in source." Source multiplies are not cost; emitted instructions are.

**Step 2 — find the scale.** Hunt for `*4`, `,4)`, or `add $4` on an address
register. The rejected wrong reading is: "the `4` in `addl $4,%eax` on the sum
is the array stride." That might just be adding the loaded value four; context
matters.

**Step 3 — `-O2` diff.** See whether the functions collapsed toward one shape or
even inlined into `main`. Record what happened without judging "better" yet.

**Step 4 — tie back to types.** Mentally swap to `long *` and predict scale 8.
You need not implement it; predicting shows you understood why `4` appeared.

## Distinctions worth keeping straight

- **Semantic equivalence vs identical asm** — not the same claim.
- **Element stride vs loop counter** — both can involve the number 4.
- **`-O0` literal translation vs `-O2` rewrite** — different teaching artifacts.
- **Array object vs pointer parameter** — parameters have already decayed.

## Check yourself

1. Why does `int` indexing often involve a scale of 4 on this machine?
2. At `-O0`, if `sum_idx` and `sum_ptr` differ in instruction count, does that
   prove different C behavior?
3. What should you look for at `-O2` regarding the two functions?
4. How can `leaq (,%reg,4)` relate to indexing without an explicit `imul`?

## Key takeaways

- Indexing and pointer walking are often equivalent; asm may still differ at
  `-O0`.
- Element size appears as a scale or address increment.
- `-O2` frequently converges equivalent loops.
- Diff listings; do not argue from source aesthetics.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info as` — addressing modes

Now open `TASK.md` and do the practice.
