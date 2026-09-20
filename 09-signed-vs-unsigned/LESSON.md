# 09 — Signed vs unsigned compares
<!-- concepts: c.p2.types.width, c.p2.types.overflow -->

The `cmp` instruction subtracts without writing the destination and sets CPU
flags. What *differs* for signed versus unsigned C types is which conditional
jump (or set) instruction you use afterward: signed comparisons consult
overflow/sign relationships (`jl`, `jg`, `jle`, `jge`), while unsigned ones
consult carry/above-below relationships (`jb`, `ja`, `jbe`, `jae`). By the end of
this lesson you should recognize those mnemonic families in GCC output and know
why a loop near the top of a type's range can behave differently in C even when
the asm "only" swapped jump letters.

## What this lesson asks of you

Implement signed and unsigned compare helpers, plus a pair of loops that exit
when an index hits a limit — once with `int`, once with `unsigned`. Prefer values
near range extremes so the conceptual difference is not abstract. Read the `.s`
and name the jump mnemonics. Practice is in `TASK.md`.

## Flags in plain language

After `cmp A, B` (AT&T: `cmp B, A` means think carefully — GNU AT&T writes
`cmp src, dst` as computing `dst - src` for flags), the flags describe that
subtraction. You do not need every flag's circuitry here. You need:

- Signed jumps care about whether the signed interpretation of the difference
  is less/greater.
- Unsigned jumps care about borrow / "above or below" as magnitudes.

The mnemonics help: `l`/`g` suggest less/greater (signed); `b`/`a` suggest
below/above (unsigned). `e` / `ne` for equality are shared because bitwise
equality does not care about signedness.

## Why C signedness shows up

When you write `if (a < b)` with `int` operands, GCC emits a signed jump family.
With `unsigned`, it emits unsigned jumps. The C abstract machine's conversion
rules matter too: mixing signed and unsigned in one expression can quietly
change which comparison you get — another reason to keep types consistent in
experiments.

Loops that count toward a limit near `UINT_MAX` or negative sentinels are where
people get burned: the jump letter in asm is a clue that the C types were not
what you assumed.

## Worked example

**The situation.** `cmp_signed` returns negative / zero / positive for `int a,b`.
`cmp_unsigned` does the analogous thing for `unsigned`. You also write
`loop_signed` / `loop_unsigned` that advance an index until it reaches a limit.

**Step 1 — read `cmp_signed`.** Find `cmp` and the following `j*` / `set*`.
List the mnemonics. The rejected wrong reading is: "`jl` means jump if the left
operand in the C expression is literally in `%rax`." Mnemonics encode flag
conditions, not C expression tree shapes; match them by working backward from
what the function must return.

**Step 2 — read `cmp_unsigned`.** You should see `b`/`a` family letters instead
of `l`/`g` for the ordering tests. The rejected wrong reading is: "unsigned
means GCC skips `cmp` and uses a different opcode entirely." Usually it is still
`cmp` (or an equivalent subtract); the *branch* changes.

**Step 3 — compare the loops.** Note the jump that terminates iteration. Try
limits that make signed overflow or unsigned wrap conceptually relevant (even if
you do not trigger UB — stay within defined territory; the point is to see the
jump family).

**Step 4 — write one sentence relating mnemonic to signedness.** That sentence
is your takeaway card for later disassembly work.

## Distinctions worth keeping straight

- **`jl`/`jg` vs `jb`/`ja`** — signed vs unsigned ordering.
- **Equality jumps** — shared across signedness.
- **C type vs asm mnemonic** — the type chose the mnemonic.
- **Flag meaning vs operand spelling in AT&T `cmp`** — read carefully.

## Check yourself

1. Which jump family should you expect for `int a < b`?
2. Which family for `unsigned a < b`?
3. Why is `je` after a compare not evidence of signedness either way?
4. If you see `ja` in a loop you thought was signed, what should you re-check in
   the C source?

## Key takeaways

- Signed and unsigned comparisons share `cmp` but differ in jump/set mnemonics.
- `l`/`g` ≈ signed; `b`/`a` ≈ unsigned; equality is common.
- Loop exit jumps reveal the types GCC believed.
- Read mnemonics from your listing; infer flags from names + practice.

## Lookup (not the lesson)

- `man 1 objdump` — `-d`
- `info as` — jump mnemonic list when you need spelling
- `man 1 gcc` — `-S`

Now open `TASK.md` and do the practice.
