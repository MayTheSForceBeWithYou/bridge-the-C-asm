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

## Address math versus arithmetic reuse

For `int *p` and index `i`, the byte address is `p + 4*i`. That may appear as
`leaq (%rdi,%rsi,4), %rax` — classic "address of element." The same hardware
mechanism can compute `8*x + y` for ordinary ints because the scale options
include 1, 2, 4, and 8. GCC often chooses `lea` when it fits, even when no
pointer exists in the C source.

`add` still appears when you simply accumulate, when scales do not fit, or when
flags from addition are needed (`lea` does not set condition flags the way
`add` does — a subtle reason compilers pick one or the other).

## Optimization level

At `-O0`, you may see more literal `imul` for `*3`. At `-O2`, strength reduction
to `lea` / shifts + adds is common. Treat both as valid lowerings of the same C.

## Reading AT&T `lea`

`leaq (%rdi,%rsi,4), %rax` means: take `%rdi`, add `%rsi` times 4, put that sum
in `%rax`. Parentheses do not imply a memory *load* here — for `lea` they wrap
the address expression being computed. Contrast with `movl (%rdi,%rsi,4), %eax`,
which *does* load from that address.

## Worked example

**The situation.** `elem(int *p, long i)` returns `p+i` as a pointer; `mix(int x,
int y)` returns `8*x+y`; `weird(int x)` returns `3*x+1`.

**Step 1 — `elem` at `-O2`.** Look for scaled `lea`. The rejected wrong reading
is: "`lea` loads from memory because its name says load." It only computes;
no memory read unless you later dereference.

**Step 2 — `mix`.** See `lea` with scale 8 or equivalent shift/add. The rejected
wrong reading is: "scale 8 means eight array elements of `int`." Scale multiplies
the index register; element size depends on context. Here it is pure arithmetic.

**Step 3 — `weird`.** Note whether `*3` became `lea` (for example `x*2+x`) plus
an add of 1, or stayed `imul`. Either can be fine — record what you got.

**Step 4 — flags thought.** If a later instruction needed flags from the add,
`lea` might be the wrong tool; keep that distinction for future reading.

## Distinctions worth keeping straight

- **`lea` computes vs `mov` from memory loads** — names mislead.
- **Pointer element addressing vs integer strength reduction** — same opcode
  family.
- **`lea` vs `add` regarding flags** — different side effects.
- **`-O0` imul vs `-O2` lea** — optimization texture.

## Check yourself

1. Does `lea` read memory at the computed address?
2. Why can `8*x+y` lower to `lea` with no pointers in sight?
3. What scale factors does classic x86 addressing allow?
4. When might GCC prefer `add` over `lea` for a simple sum?

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
