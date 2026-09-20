# 31 — Conditional move

A branch implements `if` by jumping. A *conditional move* (`cmov*`) implements
some choices by moving data only when flags say so — control flow falls through
either way. For tiny helpers like `max` or `abs`, `-O2` often prefers `cmov`
(or other branchless sequences) to avoid branch misprediction, while `-O0`
usually emits compare + jump + two paths. By the end of this lesson you should
recognize `cmov*` in optimized listings and contrast it with the `-O0` branchy
shape.

## What this lesson asks of you

Implement straightforward `int max(int a, int b)` and `int abs_int(int x)`.
Compare `make asm` versus `make O=2 asm`. Practice in `TASK.md`.

## What `cmov` needs

Like conditional jumps, `cmov*` consults flags from a prior `cmp` / `test`.
Unlike jumps, both source values are typically prepared, and only one lands in
the destination. The instruction stream is straighter; the data path carries the
choice.

Not every conditional becomes `cmov`. Complex bodies, calls, or unpredictable
patterns may stay branched. Small pure selectors are the sweet spot for this
micro-rep.

## `abs` flavors

Absolute value may appear as `cmov` after computing `-x`, as bit tricks with
sign bits, or as a branch at `-O0`. Read what GCC emitted; name it accurately
rather than forcing a `cmov` story if you got something else branchless.

## Why compilers bother

Branches are cheap when predicted well and expensive when not. For a two-way
select with tiny payloads, paying for both values and a `cmov` can be a win in
tight loops. That policy is why `-O2` may surprise you if you only memorized
`if` → `jcc` from `-O0` listings.

## Worked example

**The situation.** `max(3,7)` → 7; `abs_int(-5)` → 5. You inspect both `-O`
levels.

**Step 1 — `-O0` `max`.** Expect `cmp`, a `jcc`, and two paths that move either
`a` or `b` toward the return register. The rejected wrong reading is: "two paths
mean my function returned twice." One path runs per invocation; the listing shows
both possibilities statically.

**Step 2 — `-O2` `max`.** Hunt `cmov*` (names like `cmovg`, `cmovge`, …). The
rejected wrong reading is: "no jump means the compare disappeared." The compare
usually remains; only the control hazard shrinks.

**Step 3 — `abs_int` both levels.** Describe each lowering in one line.

**Step 4 — connect to lesson 09.** The condition letters on `cmov*` rhyme with
jump families (`g`/`l`/`a`/`b`); signed `max` should look signed.

## Distinctions worth keeping straight

- **Branchy select vs `cmov` select** — control versus data predication.
- **Flags still needed** — `cmov` does not replace `cmp`.
- **`-O0` pedagogy vs `-O2` branchless** — both correct for C.
- **Always-`cmov` myth** — compilers choose; observe.

## Check yourself

1. What does `-O0` typically emit for a simple `max`?
2. What should you search for at `-O2` as a branchless select?
3. Why might a compare still appear next to `cmov`?
4. If `-O2` still branches for your `max`, did you necessarily do something wrong
   in C?

## Key takeaways

- Conditional moves select values without jumping.
- `-O2` often uses `cmov*` for tiny selectors like `max`/`abs`.
- `-O0` usually shows explicit branches instead.
- Confirm with side-by-side `gcc -S` listings.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info as` — `cmov` mnemonic list if needed

Now open `TASK.md` and do the practice.
