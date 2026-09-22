# 30 — LEA vs ADD

`lea` (load effective address) computes an address-like arithmetic expression
and writes the result to a register *without* accessing memory. On x86-64 it is
also a favorite way to express certain integer formulas — `3*x+1`, `8*x+y`,
pointer `p+i` — in one instruction using scaled-index addressing forms. By the
end of this lesson you should spot when GCC prefers `lea` over `imul`/`add`
chains and know that `lea` is not always "about pointers" in the C sense.

## What this lesson asks of you

Write functions that do pointer `p + i` for `int *p` and integer expressions
like `3*x+1` / `8*x+y`. Compare `-O0` and `-O2` asm. `TASK.md` is the lab.

## What `make` is naming for you

| You type | Open | Search |
| -------- | ---- | ------ |
| `make asm` | `prog.s` | `elem:`, `mix:`, `weird:`, `lea` / `imul` |
| `make O=2 asm` | copy first | `lea` often appears more |
| `make disasm` | `prog.lst` | `<elem>:` |

## Address math versus arithmetic reuse

For `int *p` and index `i`, the byte address is `p + 4*i`. That may appear as
`leaq (%rdi,%rsi,4), %rax` — classic "address of element." The same hardware
mechanism can compute `8*x + y` for ordinary ints because the scale options
include 1, 2, 4, and 8. GCC often chooses `lea` when it fits, even when no
pointer exists in the C source.

### Decode one `lea` line (AT&T)

```text
        leaq    (%rdi,%rsi,4), %rax
```

| Piece | Meaning |
| ----- | ------- |
| `lea` / `leaq` | compute effective address; **no memory read** |
| `%rdi` | base |
| `%rsi` | index |
| `4` | scale (1, 2, 4, or 8) |
| `%rax` | destination receiving `base + index*scale (+ disp)` |

Contrast with a load from the same address expression:

```text
        movl    (%rdi,%rsi,4), %eax   # DOES read memory at that address
```

| Instruction | Memory access? | Typical C reading |
| ----------- | -------------- | ----------------- |
| `leaq (%rdi,%rsi,4), %rax` | No | `&p[i]` or pointer `p+i` |
| `movl (%rdi,%rsi,4), %eax` | Yes | `p[i]` value |

**Rejected wrong reading:** "`lea` loads from memory because its name says
load." It only computes; no memory read unless you later dereference.

### Pure arithmetic `lea` at `-O2`

Real GCC `-O2` shapes:

```text
elem:   # int *p, long i → p+i
        leaq    (%rdi,%rsi,4), %rax
        ret

mix:    # 8*x + y
        leal    (%rsi,%rdi,8), %eax
        ret

weird:  # 3*x + 1
        leal    1(%rdi,%rdi,2), %eax   # 1 + x + x*2
        ret
```

| C expression | Scale / form | Pointer in source? |
| ------------ | ------------ | ------------------ |
| `p + i` for `int *` | scale 4 | Yes |
| `8*x + y` | scale 8 | No |
| `3*x + 1` | `x + 2*x + 1` | No |

**Rejected wrong reading:** "scale 8 means eight array elements of `int`." Scale
multiplies the index register; element size depends on context. In `mix` it is
pure arithmetic.

## `lea` vs `add` regarding flags

`add` sets condition flags (ZF, SF, CF, …). `lea` does **not** set flags the way
`add` does. Compilers may prefer `add` when a later instruction needs those
flags, and `lea` when they want arithmetic without clobbering flags — or simply
when the scaled form is a neat fit.

## Optimization level

At `-O0`, you may see more literal `imul` for `*3`. At `-O2`, strength reduction
to `lea` / shifts + adds is common. Treat both as valid lowerings of the same C.

### Navigation

1. Search each function label at `-O0`; note `imul` / `add` / `lea`.
2. Repeat at `-O2`; highlight new `lea` forms.
3. For every `lea`, write the algebraic expression it computes.
4. Mark whether that expression came from a pointer in C or pure ints.

## Worked example

**The situation.** `elem(int *p, long i)` returns `p+i` as a pointer; `mix(int x,
int y)` returns `8*x+y`; `weird(int x)` returns `3*x+1`.

**Step 1 — `elem` at `-O2`.** Look for scaled `lea`. Confirm no memory load.

**Step 2 — `mix`.** See `lea` with scale 8 or equivalent shift/add.

**Step 3 — `weird`.** Note whether `*3` became `lea` (for example `x*2+x`) plus
an add of 1, or stayed `imul`. Either can be fine — record what you got.

**Step 4 — flags thought.** If a later instruction needed flags from the add,
`lea` might be the wrong tool; keep that distinction for future reading.


## Algebra drill on one line

For each `lea` you find, expand it:

```text
leal 1(%rdi,%rdi,2), %eax  →  eax = 1 + rdi + rdi*2 = 3*rdi + 1
leaq (%rdi,%rsi,4), %rax   →  rax = rdi + rsi*4
leal (%rsi,%rdi,8), %eax   →  eax = rsi + rdi*8
```

If you cannot expand it, you do not yet read `lea` — fix that before arguing
about optimization.


## `-O0` may still `imul`

If `weird` at `-O0` shows `imul` by 3, that is expected pedagogy. The interesting
delta is often `-O2` rewriting it to `lea`. Document both; the lesson is the
contrast, not that every build uses `lea` for everything.


## Scale factor table

| Scale | Common C meanings |
| ----- | ----------------- |
| 1 | `p + i` for `char *`, or `x + y` forms |
| 2 | `short *` indexing / `2*x` |
| 4 | `int *` indexing / `4*x` |
| 8 | `long *` / pointer-sized / `8*x` |

Match scale to *your* expression, not to a single memorized story.

## Distinctions worth keeping straight

- **`lea` computes vs `mov` from memory loads** — names mislead.
- **Pointer element addressing vs integer strength reduction** — same opcode
  family.
- **`lea` vs `add` regarding flags** — different side effects.
- **Scale 4 for `int *` vs scale 8 for `8*x`** — do not confuse.
- **`-O0` imul vs `-O2` lea** — optimization texture.

## Check yourself

1. Does `lea` read memory at the computed address?
2. Why can `8*x+y` lower to `lea` with no pointers in sight?
3. What scale factors does classic x86 addressing allow?
4. When might GCC prefer `add` over `lea` for a simple sum?
5. How do you tell `leaq (%rdi,%rsi,4),%rax` from `movl (%rdi,%rsi,4),%eax`?
6. What algebraic expression is `leal 1(%rdi,%rdi,2), %eax`?

## Key takeaways

- `lea` computes effective addresses (and handy integer formulas) without memory
  access.
- Scaled-index forms explain both `p+i` and many `k*x+y` patterns.
- `-O2` often rewrites multiplies into `lea`/shift/add mixes.
- Read the instruction; do not assume `lea` implies a C pointer.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info as` — addressing modes

Now open `TASK.md` and do the practice.
