# 09 — Signed vs unsigned compares

The `cmp` instruction subtracts without writing the destination and sets CPU
flags. What *differs* for signed versus unsigned C types is which conditional
jump (or set) instruction you use afterward: signed comparisons consult
overflow/sign relationships (`jl`, `jg`, `jle`, `jge`), while unsigned ones
consult carry/above-below relationships (`jb`, `ja`, `jbe`, `jae` / `jnb`). By
the end of this lesson you should recognize those mnemonic families in GCC
output and know why a loop near the top of a type's range can behave differently
in C even when the asm "only" swapped jump letters.

## What this lesson asks of you

Implement signed and unsigned compare helpers, plus a pair of loops that exit
when an index hits a limit — once with `int`, once with `unsigned`. Prefer values
near range extremes so the conceptual difference is not abstract. Read the `.s`
and name the jump mnemonics. Practice is in `TASK.md`.

## What `make` is naming for you

| You type | Open |
| -------- | ---- |
| `make asm` | `prog.s` — search `cmp_signed:` / `cmp_unsigned:` |
| `make disasm` | `prog.lst` — same mnemonics after link |

## Flags in plain language

After `cmp`, the flags describe a subtraction used only for condition codes. In
GNU AT&T syntax, `cmp src, dst` computes flags as for `dst - src` (destination
minus source) without storing the difference. Read carefully — operand order
trips people who learned Intel syntax first.

You do not need every flag's circuitry here. You need:

- Signed jumps care about whether the signed interpretation of the difference
  is less/greater.
- Unsigned jumps care about borrow / "above or below" as magnitudes.

### Mnemonic cheat sheet (recognition, not memorization drill)

| Family | Mnemonics you will see | C-shaped meaning |
| ------ | ---------------------- | ---------------- |
| Signed order | `jl`, `jle`, `jg`, `jge` (and `setl` …) | less / greater for signed |
| Unsigned order | `jb`, `jbe`, `ja`, `jae` / `jnb`, `jnb`… | below / above for unsigned |
| Equality | `je` / `jz`, `jne` / `jnz` | equal / not — shared |

The letters help: `l`/`g` suggest less/greater (signed); `b`/`a` suggest
below/above (unsigned). `e` / `ne` for equality are shared because bitwise
equality does not care about signedness.

### Real `-O0` contrast

```text
# int a < b path often uses signed letters:
        cmpl    -8(%rbp), %eax
        jge     .L2          # jump if NOT (a < b) in this shape
        …

# unsigned a < b path uses below/above letters:
        cmpl    -8(%rbp), %eax
        jnb     .L6          # jump if not below (unsigned)
        …
```

| What to write down | Example |
| ------------------ | ------- |
| The `cmp` / `cmpl` line | operands + AT&T order reminder |
| The *next* conditional jump | `jge` vs `jnb` |
| Which C function it came from | `cmp_signed` vs `cmp_unsigned` |

**Rejected wrong reading:** "`jl` means jump if the left operand in the C
expression is literally in `%rax`." Mnemonics encode flag conditions, not C
expression tree shapes; match them by working backward from what the function
must return.

**Rejected wrong reading:** "unsigned means GCC skips `cmp` and uses a different
opcode entirely." Usually it is still `cmp` (or an equivalent subtract); the
*branch* changes.

## Why C signedness shows up

When you write `if (a < b)` with `int` operands, GCC emits a signed jump family.
With `unsigned`, it emits unsigned jumps. The C abstract machine's conversion
rules matter too: mixing signed and unsigned in one expression can quietly
change which comparison you get — another reason to keep types consistent in
experiments.

Loops that count toward a limit near `UINT_MAX` or negative sentinels are where
people get burned: the jump letter in asm is a clue that the C types were not
what you assumed.

### Navigation for the loop pair

1. Search `loop_signed:` / `loop_unsigned:`.
2. Find the `cmp`/`test` that decides whether to keep iterating.
3. Name the `j*` that exits (or continues).
4. Tie that mnemonic back to the C index type.

## Worked example

**The situation.** `cmp_signed` returns negative / zero / positive for `int a,b`.
`cmp_unsigned` does the analogous thing for `unsigned`. You also write
`loop_signed` / `loop_unsigned` that advance an index until it reaches a limit.

**Step 1 — read `cmp_signed`.** Find `cmp` and the following `j*` / `set*`.
List the mnemonics.

**Step 2 — read `cmp_unsigned`.** You should see `b`/`a` / `nb` family letters
instead of `l`/`g` for the ordering tests.

**Step 3 — compare the loops.** Note the jump that terminates iteration. Stay
within defined C behavior; the point is to see the jump family.

**Step 4 — write one sentence relating mnemonic to signedness.** That sentence
is your takeaway card for later disassembly work.


## AT&T `cmp` operand order — one careful example

Intel manuals think `CMP dest, src` differently from GNU AT&T. In this track's
`.s` files you will see:

```text
        cmpl    %eax, -8(%rbp)
```

Flags are set as for `(-8(%rbp)) - %eax` (destination minus source in AT&T's
`cmp src, dst` spelling). When matching to C `a < b`, use the *function's
result* and verbose comments to see which branch is which — do not only memorize
operand order. The mnemonic family (`jl` vs `jb`) still tells you signed vs
unsigned even when paths are inverted (`jge` meaning "skip the less-than body").

### Loop exit as a type detector

If you believed both loops used `int` but one exits with `ja` / `jnb`, re-read
the C declarators. The jump letter is often the fastest way to catch a silent
`unsigned` change.


## `set*` cousins

Sometimes GCC uses `setl` / `setb` into a byte register instead of branching,
then moves that into the return value. The letter family is the same story as
jumps: `l` signed, `b` unsigned. If you see `set*` instead of `j*`, still record
the mnemonic family — do not claim "no compare happened."

### Mixing types (what not to do in the experiment)

If you write `if (signed_var < unsigned_var)`, C's usual arithmetic conversions
may force an unsigned compare. Your asm will then show `b`/`a` letters even
though one operand "looked signed" in source. Keep types consistent in this lab
so the mnemonic maps cleanly to the lesson.

## Distinctions worth keeping straight

- **`jl`/`jg` vs `jb`/`ja`/`jnb`** — signed vs unsigned ordering.
- **Equality jumps** — shared across signedness.
- **C type vs asm mnemonic** — the type chose the mnemonic.
- **Flag meaning vs operand spelling in AT&T `cmp`** — `cmp src,dst` ≠ Intel order.
- **`jge` as "not less"** — inverted conditions are common; read the path.

## Check yourself

1. Which jump family should you expect for `int a < b`?
2. Which family for `unsigned a < b`?
3. Why is `je` after a compare not evidence of signedness either way?
4. If you see `ja` in a loop you thought was signed, what should you re-check in
   the C source?
5. In AT&T `cmpl %eax, -8(%rbp)`, which operand is the "destination" for flag
   math?
6. What search string finds your compare helper quickly?

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
