# 24 — Summing with labels and conditional jumps

C `for` and `while` are illusions over compare-and-branch. In assembly you name
labels, subtract or compare, and jump — or you fall through. This lesson builds
`sum_to(n)` as an explicit loop so control flow is something you can point at in
an `.s` file, not a keyword the compiler invents. By the end you should decode
AT&T `cmp` operand order, name which path is fall-through, and find your loop's
`cmp`/`jcc` pair in a listing without guessing.

## What this lesson asks of you

Implement `long sum_to(long n)` returning `1+2+…+n` with `sum_to(0) == 0`, using
`cmp` / conditional jumps / `jmp` as needed. Tests assume `n >= 0`. No C control
flow inside the routine — the harness is C; your function is pure asm.

## Anatomy of an asm loop

A loop needs five moving parts. Name them so a listing is not a blur:

| Part | Typical instructions | Job |
| ---- | -------------------- | --- |
| Initialization | `xor %rax,%rax`, copy `%rdi` | Clear accumulator; set counter/limit |
| Test | `cmp` / `test` | Write condition flags |
| Conditional branch | `jcc` (`je`, `jg`, `jle`, …) | Take an edge if flags match |
| Body | `add`, `inc` / `dec` | Update sum and induction variable |
| Back-edge | `jmp` or backward `jcc` | Return to the test or body |

### Decoding `cmp` in AT&T (the junior trap)

AT&T: `cmpq %rsi, %rdi` sets flags as if **destination minus source** —
conceptually `rdi - rsi`. Getting the operand order wrong flips every branch.

| You write | Flags as if | So `jg` means… |
| --------- | ----------- | -------------- |
| `cmpq %rsi, %rdi` | `rdi - rsi` | jump if `rdi > rsi` |
| `cmpq %rdi, %rsi` | `rsi - rdi` | jump if `rsi > rdi` (opposite sense) |

**Recognition rule:** read `cmp src, dst` as "compare dst against src by
computing dst−src." Then pick `jcc` for the inequality you actually want.

A listing fragment might look like:

```text
    1188:	48 39 f8             	cmp    %rdi,%rax
    118b:	7e 0a                	jle    1197 <sum_to+0x2f>
```

| Column | Example | Meaning |
| ------ | ------- | ------- |
| Address | `1188:` | Where the `cmp` lives |
| Bytes | `48 39 f8` | Encoding |
| Mnemonic | `cmp %rdi,%rax` | flags as `rax - rdi` |
| Next line `jle` | `jle 1197 <…>` | taken if `rax ≤ rdi`; `1197` is the target address |

**Rejected wrong reading:** "AT&T `cmp` is the same operand order as Intel
syntax notes I memorized." Intel's spelling differs; copying Intel order into a
GAS file reverses your branches.

**Rejected wrong reading:** "`7e 0a` on the `jle` line is the sum." Those are
machine bytes for the short jump; the sum lives in a register (hopefully
`%rax`) when you finally `ret`.

## Choosing a loop shape

Two honest shapes:

- **Count up:** `i = 1; while (i <= n) { sum += i; i++; }`
- **Count down:** add `n`, then `n--`, until `n` hits zero (careful with
  `sum_to(0)`).

Either is fine. What matters is that the zero case does not fall into a body
that adds once by accident, and that the sum ends in `%rax` for the C caller.

GAS local labels (`.Lloop`, `.Ldone`) keep the symbol table clean: only
`sum_to` needs to be `.globl`. In `objdump -d` those often appear as
`<sum_to+0x..>` offsets rather than pretty names — **search `<sum_to>:` first**,
then read forward for `jcc` / `jmp`.

## Flags are ephemeral

`cmp` and arithmetic instructions update condition codes. Write the `jcc`
immediately after the compare you mean, or you will branch on stale or wrong
flags. Reading Track A exercise 08 `.s` output shows how `gcc` places
`cmp`/`jcc` pairs; match that discipline by hand.

### Fall-through is a real path

When a conditional jump is **not** taken, execution continues at the next
instruction. That next instruction is not "dead"; it is the other arm of your
`if`. Sketch both edges on paper before coding.

**Rejected wrong reading:** "I only enter labels by jumping." Fall-through into
the instruction after a failed `jcc` is how most loops and if/else shapes work.

## What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | harness checks several `n` including 0 |
| `objdump -d` | search `<sum_to>:`; note `cmp`/`jcc` pairs |
| Revisit ex. 08 `gcc -S` | compiler's jump shapes as reference |

## Worked example

**The situation.** Stub `sum_to.s` must satisfy the harness for several `n`,
including zero.

**Step 1 — decide registers.** Example plan: `%rdi` holds `n` (or a copy),
`%rax` accumulates, a third register counts. The rejected wrong reading is:
"I can leave the sum in `%rdi` because that was the input." The caller still
reads `%rax` after `ret`.

**Step 2 — write the zero short-circuit or a test-at-top loop.** For `n == 0`,
return 0 without adding. Sketch labels like `sum_to`, `.Lloop`, `.Ldone`. The
rejected wrong reading is: "labels are only for functions." Local labels are
how you structure control flow inside one symbol.

**Step 3 — check known values mentally.** `sum_to(0) → 0`, `sum_to(1) → 1`,
`sum_to(3) → 6`. If `sum_to(1)` fails, your branch probably treats equality
wrong (`jle` vs `jl`). If large `n` fails, look for width mistakes (`l` vs `q`).

**Step 4 — read your own listing.** Search `<sum_to>:`. Find each `cmp` and the
immediately following `jcc`. Using the AT&T decode table, say out loud which
inequality that pair implements. If it is not the inequality in your C sketch,
fix the operands or the condition code — do not "add a second jump" at random.

## Distinctions worth keeping straight

- **`cmp a,b` then `jcc`** — know which operand is subtracted from which in
  AT&T.
- **Conditional jump vs `jmp`** — one reads flags; the other always goes.
- **Fall-through vs labeled target** — both are control-flow edges.
- **Instruction address vs jump target** — left column of `jcc` vs the operand
  address / `<sum_to+…>` hint.
- **Closed form `n(n+1)/2`** — fine mathematically; this exercise wants a loop
  so you practice jumps.
- **Return still in `%rax`** — loops do not invent a new ABI.

## Check yourself

1. What does `cmpq %rax, %rdi` compare, in which order?
2. Where must the final sum reside for C?
3. How should `sum_to(0)` behave, and which kind of jump/path makes that happen?
4. Why put `jcc` immediately after `cmp` in careful code?
5. After a `jcc` that is *not* taken, what executes next?
6. On a `jle 1197 <sum_to+0x2f>` line, which token is the instruction address
   of the jump itself, and which is the target?
7. What search string finds your function in `objdump -d`?

## Key takeaways

- Loops in asm are labels plus `cmp`/`jcc`/`jmp`, not language keywords.
- AT&T `cmp` operand order controls which way your inequalities face.
- Fall-through is a path you must design, not an accident.
- Hex on a jump line is not all one kind of number — address vs target vs bytes.
- Return values still follow the ABI (`%rax`) after the loop exits.
- Zero and one are the cases that catch off-by-one branches fastest.


## Connecting to Track A jumps

Exercise 08 showed compiler-emitted `cmp`/`jcc` pairs. Open one of those `.s`
or `.lst` files and search for a `cmp` immediately followed by a `j`. Copy the
*discipline* (adjacent compare and branch), not necessarily the register
allocation. Your hand-written loop should be readable the same way: a reader
who finds `<sum_to>:` should see the test, the taken edge, the body, and the
back-edge without hunting across fifty unrelated instructions.

If your listing scatters a `cmp` far from its `jcc` with arithmetic in between
that quietly updates flags, you have invented a bug class compilers usually
avoid at `-O0`. Prefer boring adjacency.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Jump shapes from exercise 08 compiler output
- `man 1 objdump` — listing flags

Now open `TASK.md` and do the practice.
