# 24 — Summing with labels and conditional jumps

C `for` and `while` are illusions over compare-and-branch. In assembly you name
labels, subtract or compare, and jump — or you fall through. This lesson builds
`sum_to(n)` as an explicit loop so control flow is something you can point at in
an `.s` file, not a keyword the compiler invents.

## What this lesson asks of you

Implement `long sum_to(long n)` returning `1+2+…+n` with `sum_to(0) == 0`, using
`cmp` / conditional jumps / `jmp` as needed. Tests assume `n >= 0`. No C control
flow inside the routine — the harness is C; your function is pure asm.

## Anatomy of an asm loop

A loop needs:

1. **Initialization** — clear an accumulator (often `%rax` or `%rcx`), set a
   counter or limit from the argument in `%rdi`.
2. **A header or tail test** — `cmp` (or `test`) that writes flags.
3. **A conditional jump** — `je` / `jg` / `jl` / `jle` / `jne` and kin, each
   reading those flags.
4. **A body** — add into the accumulator, bump the counter.
5. **A back-edge** — `jmp` to the test, or a compare at the bottom with a
   backward conditional jump.

AT&T syntax: `cmpq %rsi, %rdi` sets flags as if `rdi - rsi` (destination minus
source). Getting the operand order wrong flips every branch — the classic junior
mistake when porting from Intel syntax notes.

## Choosing a loop shape

Two honest shapes:

- **Count up:** `i = 1; while (i <= n) { sum += i; i++; }`
- **Count down:** add `n`, then `n--`, until `n` hits zero (careful with
  `sum_to(0)`).

Either is fine. What matters is that the zero case does not fall into a body
that adds once by accident, and that the sum ends in `%rax` for the C caller.

You may keep the limit in a callee-saved register if the body needs many
scratches — then push/pop around the function. For a tiny loop, caller-saved
registers alone usually suffice.

## Flags are ephemeral

`cmp` and arithmetic instructions update condition codes. A `mov` in between
does not clear them on x86, but many other ops do. Write the `jcc` immediately
after the compare you mean, or you will branch on stale or wrong flags. Reading
Track A exercise 08 `.s` output shows how `gcc` places `cmp`/`jcc` pairs; match
that discipline by hand.

## Worked example

**The situation.** Stub `sum_to.s` must satisfy the harness for several `n`,
including zero.

**Step 1 — decide registers.** Example plan: `%rdi` holds `n` (or a copy),
`%rax` accumulates, a third register counts. The rejected wrong reading is:
"I can leave the sum in `%rdi` because that was the input." The caller still
reads `%rax` after `ret`.

**Step 2 — write the zero short-circuit or a test-at-top loop.** For `n == 0`,
return 0 without adding. Sketch labels like `sum_to`, `.Lloop`, `.Ldone` (local
labels with `.L` are a common GAS habit). The rejected wrong reading is: "labels
are only for functions." Local labels are how you structure control flow inside
one symbol.

**Step 3 — check a known value.** Mentally execute `sum_to(3)`: expect 6. Then
`make && make run`. If `sum_to(1)` fails, your branch probably treats equality
wrong; if large `n` fails, look for width mistakes (`l` vs `q`) or sign issues
(this exercise stays non-negative).

## Distinctions worth keeping straight

- **`cmp a,b` then `jcc`** — know which operand is subtracted from which in
  AT&T.
- **Conditional jump vs `jmp`** — one reads flags; the other always goes.
- **Fall-through** — the instruction after a failed `jcc` is a real path; do not
  assume you only enter labels by jumping.
- **Closed form `n(n+1)/2`** — fine mathematically; this exercise wants a loop
  so you practice jumps.

## Check yourself

1. What does `cmpq %rax, %rdi` compare, in which order?
2. Where must the final sum reside for C?
3. How should `sum_to(0)` behave, and which jump makes that happen?
4. Why put `jcc` immediately after `cmp` in careful code?

## Key takeaways

- Loops in asm are labels plus `cmp`/`jcc`/`jmp`, not language keywords.
- AT&T `cmp` operand order controls which way your inequalities face.
- Return values still follow the ABI (`%rax`) after the loop exits.
- Zero and one are the cases that catch off-by-one branches fastest.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Jump shapes from exercise 08 compiler output

Now open `TASK.md` and do the practice.
