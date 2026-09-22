# 31 — Conditional move

A branch implements `if` by jumping. A *conditional move* (`cmov*`) implements
some choices by moving data only when flags say so — control flow falls through
either way. For tiny helpers like `max` or `abs`, `-O2` often prefers `cmov`
(or other branchless sequences) to avoid branch misprediction, while `-O0`
usually emits compare + jump + two paths — though some GCC versions already emit
`cmov` at `-O0` for tiny selects. By the end of this lesson you should recognize
`cmov*` in listings and contrast branchy vs branchless shapes when both appear.

## What this lesson asks of you

Implement straightforward `int max(int a, int b)` and `int abs_int(int x)`.
Compare `make asm` versus `make O=2 asm`. Practice in `TASK.md`.

## What `make` is naming for you

| You type | Open | Search |
| -------- | ---- | ------ |
| `make asm` | `prog.s` | `max:`, `abs_int:`, `j` / `cmov` |
| `make O=2 asm` | copy first | expect `cmov*` more often |
| `make disasm` | `prog.lst` | `<max>:` |

## What `cmov` needs

Like conditional jumps, `cmov*` consults flags from a prior `cmp` / `test`.
Unlike jumps, both source values are typically prepared, and only one lands in
the destination. The instruction stream is straighter; the data path carries the
choice.

### Decode a branchless `max`

Real GCC shape (often even at `-O0` on recent compilers):

```text
max:
        …
        cmpl    %edi, %esi
        movl    %edi, %eax
        cmovge  %esi, %eax
        ret
```

| Instruction | Role |
| ----------- | ---- |
| `cmpl %edi, %esi` | compare `b` vs `a` (AT&T: flags for `esi - edi`…) |
| `movl %edi, %eax` | default result ← `a` |
| `cmovge %esi, %eax` | if condition (signed ≥), result ← `b` instead |

| Piece | Recognition rule |
| ----- | ---------------- |
| `cmov*` mnemonic | conditional move — note the condition letters (`ge`, `g`, `l`, …) |
| Prior `cmp`/`test` | still required — flags do not appear from nowhere |
| No `jcc` to a second return path | branchless select |

### Branchy shape (when you see it)

```text
        cmpl    …
        jle     .L_b
        movl    a, %eax
        jmp     .L_done
.L_b:
        movl    b, %eax
.L_done:
        ret
```

| Shape | Control flow | Data |
| ----- | ------------ | ---- |
| Branchy | jumps choose a path | one path runs |
| `cmov` | fall through always | move may or may not happen |

**Rejected wrong reading:** "two paths in the listing mean my function returned
twice." One path runs per invocation; the listing shows both possibilities
statically.

**Rejected wrong reading:** "no jump means the compare disappeared." The compare
usually remains; only the control hazard shrinks.

## `abs` flavors

Absolute value may appear as `cmov` after computing `-x`:

```text
abs_int:
        movl    %edi, %eax
        negl    %eax
        cmovs   %edi, %eax    # if neg set sign flag funny — read *your* flags story
        ret
```

Or as bit tricks with sign bits, or as a branch at some `-O` levels. Read what
GCC emitted; name it accurately rather than forcing a `cmov` story if you got
something else branchless.

### Navigation

1. Search `max:` at `-O0` — list `cmp`, `j*`, `cmov*`.
2. Repeat at `-O2` — note what changed.
3. Repeat for `abs_int`.
4. Connect condition letters to lesson 09 (`g`/`l` signed vs `a`/`b` unsigned).
   Signed `max` should look signed.

## Why compilers bother

Branches are cheap when predicted well and expensive when not. For a two-way
select with tiny payloads, paying for both values and a `cmov` can be a win in
tight loops. That policy is why `-O2` may surprise you if you only memorized
`if` → `jcc` from older `-O0` listings.

Not every conditional becomes `cmov`. Complex bodies, calls, or unpredictable
patterns may stay branched. Small pure selectors are the sweet spot for this
micro-rep.

## Worked example

**The situation.** `max(3,7)` → 7; `abs_int(-5)` → 5. You inspect both `-O`
levels.

**Step 1 — `-O0` `max`.** Expect either branchy paths or an early `cmov` —
record *your* compiler's choice with citations.

**Step 2 — `-O2` `max`.** Hunt `cmov*` (names like `cmovg`, `cmovge`, …).

**Step 3 — `abs_int` both levels.** Describe each lowering in one line.

**Step 4 — connect to lesson 09.** The condition letters on `cmov*` rhyme with
jump families; signed `max` should look signed.


## Condition letter ↔ jump family

| `cmov` / `j` letter | Signed story | Unsigned story |
| ------------------- | ------------ | -------------- |
| `l` / `le` / `g` / `ge` | yes | no |
| `b` / `be` / `a` / `ae` | no | yes |
| `e` / `z` / `ne` / `nz` | equality — either | either |
| `s` / `ns` | sign flag tricks (often `abs`) | — |

When reading `cmovge` in `max`, you are in the signed family — appropriate for
`int`.


## If both levels already use `cmov`

Say so in your notes, then still answer: where do flags come from (`cmp`/`test` /
`neg`)? What condition letters appear? How would a branchy shape have looked
(sketch it) even if GCC did not emit one? That keeps the competence about
*recognition*, not about forcing a historical `-O0` branch.

## Distinctions worth keeping straight

- **Branchy select vs `cmov` select** — control versus data predication.
- **Flags still needed** — `cmov` does not replace `cmp`.
- **`-O0` may already `cmov` on modern GCC** — do not panic; still compare to
  `-O2` and to any branchy shape you find.
- **Always-`cmov` myth** — compilers choose; observe.
- **Condition letters match signedness** — same family as lesson 09 jumps.

## Check yourself

1. What does a simple `max` often look like when branchy?
2. What should you search for as a branchless select?
3. Why might a compare still appear next to `cmov`?
4. If `-O2` still branches for your `max`, did you necessarily do something wrong
   in C?
5. If `-O0` already shows `cmovge`, is the lesson "broken"?
6. Which letters on `cmov*` suggest signed vs unsigned conditions?

## Key takeaways

- Conditional moves select values without jumping.
- `-O2` often uses `cmov*` for tiny selectors like `max`/`abs` — and modern
  `-O0` sometimes does too.
- `cmp`/`test` still feed flags; `cmov` is not a substitute for comparing.
- Confirm with side-by-side `gcc -S` listings; cite what you actually got.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info as` — `cmov` mnemonic list if needed

Now open `TASK.md` and do the practice.
