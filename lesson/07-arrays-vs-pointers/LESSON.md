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

## What `make` is naming for you

| You type | Open | Tip |
| -------- | ---- | --- |
| `make asm` | `prog.s` | Copy to `prog-O0.s` before `make O=2 asm` |
| `make O=2 asm` | `prog.s` | Compare `sum_idx` / `sum_ptr` bodies |
| `make disasm` | `prog.lst` | Search `<sum_idx>:` / `<sum_ptr>:` |

## What indexing means in asm

Accessing an `int` element usually multiplies the index by 4 (the element size)
and adds that to the base address. That may appear as:

- Explicit `imul` / shifts + `add`, or
- A single addressing mode / `leaq` with scale.

Real `-O0` fragment for `a[i]` style:

```text
        movl    -8(%rbp), %eax      # i
        leaq    0(,%rax,4), %rdx    # i * 4
        movq    -24(%rbp), %rax     # base a
        addq    %rdx, %rax          # &a[i]
        movl    (%rax), %eax        # load a[i]
        addl    %eax, -4(%rbp)      # s += …
```

| Piece | Meaning |
| ----- | ------- |
| `leaq 0(,%rax,4), %rdx` | Compute `0 + %rax*4` into `%rdx` — **no memory load**. Scale 4 = `sizeof(int)`. |
| `movl (%rax), %eax` | *That* is the load from the element address. |
| `addl $1, -8(%rbp)` | Loop counter bump — the `1` is not the stride. |

### The number-4 trap

**Rejected wrong reading:** "the `4` in `addl $4,%eax` on the sum is the array
stride." That might just be adding the loaded *value* four, or advancing a
pointer. Context matters:

| Pattern | Likely meaning |
| ------- | -------------- |
| `,4)` or `*4` in `lea` / addressing | element scale for `int` |
| `addq $4, %reg` on a pointer | advance one `int *` |
| `addl $4, %eax` when `%eax` holds the sum | probably adding data value 4, not stride |

## Pointer increment style

Advancing a pointer by one `int` adds 4 to the address. At `-O0`, that often
shows as a distinct add to a pointer local each iteration, with a load through
it. It can look *longer* than indexing even when it is semantically equivalent,
because `-O0` is not trying to impress you with elegance.

```text
        movq    -16(%rbp), %rax   # p
        leaq    4(%rax), %rdx     # p+1 as pointer
        …                         # store updated p
        movl    (%rax), %eax      # *old_p
```

## Optimization may erase the difference

At `-O2`, both loops may become the same strength-reduced form. A real GCC `-O2`
pair often looks like:

```text
sum_idx:
        leaq    (%rdi,%rsi,4), %rdx   # end = a + n
        …
        addl    (%rdi), %eax
        addq    $4, %rdi
        …

sum_ptr:
        leaq    (%rdi,%rsi,4), %rdx
        …
        addq    $4, %rdi
        addl    -4(%rdi), %eax
```

Different instruction texture, same idea: walk by 4 bytes until `end`. They
might also vectorize later in the track; here you only need to notice convergence
or remaining cosmetic differences.

**Rejected wrong reading:** "if the asm differs, the C semantics differ."
Equivalent C can have different instruction textures, especially at `-O0`.

**Rejected wrong reading:** "pointer style is always faster because it shows
fewer multiplies in source." Source multiplies are not cost; emitted instructions
are.

## Worked example

**The situation.** Both functions sum the same array contents; `main` checks
they agree.

**Step 1 — `-O0` diff.** Place the two function bodies side by side. Note
whether instruction sequences are identical or only equivalent.

**Step 2 — find the scale.** Hunt for `*4`, `,4)`, or `addq $4` on an address
register. Label each hit as scale / pointer bump / data.

**Step 3 — `-O2` diff.** See whether the functions collapsed toward one shape or
even inlined into `main`. Record what happened without judging "better" yet.

**Step 4 — tie back to types.** Mentally swap to `long *` and predict scale 8.
You need not implement it; predicting shows you understood why `4` appeared.


## Side-by-side annotation habit

When you diff `sum_idx` and `sum_ptr`, keep a three-column note:

| Role | `sum_idx` evidence | `sum_ptr` evidence |
| ---- | ------------------ | ------------------ |
| Base pointer | which reg / slot | which reg / slot |
| Stride | `,4)` / `*4` / `add $4` | `addq $4` on pointer |
| Accumulate | `addl` into sum | `addl` into sum |
| Trip count / end test | `cmp` vs `n` | `cmp` vs end pointer |

If the rows match in *role* but not in *instruction text*, you have equivalent
loops — the lesson's point.

### Predicting `long *`

Mentally replace `int` with `long`. Scale / bump should become 8. You need not
implement it in this lab; writing the prediction proves you know why `4`
appeared for `int`.


## Makefile reminder for two optimization levels

```bash
make asm                    # writes prog.s at -O0
cp prog.s prog-O0.s
make O=2 asm                # overwrites prog.s
cp prog.s prog-O2.s
diff -u prog-O0.s prog-O2.s | less
```

When diffing, skip the file header noise and jump to `sum_idx:` / `sum_ptr:` in
each file. You care about those bodies, not whether a `.file` directive shifted.

## Distinctions worth keeping straight

- **Semantic equivalence vs identical asm** — not the same claim.
- **Element stride vs loop counter vs data value** — all can involve the number 4.
- **`lea` address math vs `mov` from memory** — compute versus load.
- **`-O0` literal translation vs `-O2` rewrite** — different teaching artifacts.
- **Array object vs pointer parameter** — parameters have already decayed.

## Check yourself

1. Why does `int` indexing often involve a scale of 4 on this machine?
2. At `-O0`, if `sum_idx` and `sum_ptr` differ in instruction count, does that
   prove different C behavior?
3. What should you look for at `-O2` regarding the two functions?
4. How can `leaq (,%reg,4)` relate to indexing without an explicit `imul`?
5. How do you tell a pointer `+4` from "add four to the sum"?
6. What search strings find each function in `.s` vs `.lst`?

## Key takeaways

- Indexing and pointer walking are often equivalent; asm may still differ at
  `-O0`.
- Element size appears as a scale or address increment — decode carefully.
- `-O2` frequently converges equivalent loops into similar walks-by-4.
- Diff listings; do not argue from source aesthetics.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info as` — addressing modes

Now open `TASK.md` and do the practice.
