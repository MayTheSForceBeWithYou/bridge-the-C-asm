# 14 — Tail-call optimization?

A call is in *tail position* when the caller immediately returns its result —
nothing remains to do after the callee comes back. Compilers may turn such a
call into a `jmp`, reusing the current frame instead of nesting another. Classic
`n * fac(n-1)` is *not* tail recursion because the multiply happens after the
call. An accumulator helper `fac_tail(n, acc)` can be. By the end of this lesson
you should see whether `-O2` rewrote your tail version into a jump or a loop-like
shape and explain why the naive version still needs post-call work in the C
abstract machine — even when GCC rewrites *both* into loops.

## What this lesson asks of you

Implement `fac_naive` and `fac_tail`, compare `make asm` versus `make O=2 asm`,
and confirm with `objdump -d`. `TASK.md` is the practice.

## What `make` is naming for you

| You type | File | Reminder |
| -------- | ---- | -------- |
| `make asm` | `prog.s` at `-O0` | Expect honest `call` in both |
| Copy aside | `prog-O0.s` | `O=2` overwrites |
| `make O=2 asm` | `prog.s` | Hunt `call` vs `jmp` vs loop |
| `make O=2 disasm` | `prog.lst` | Search `<fac_naive>:` / `<fac_tail>:` |

## What "tail position" means in C

```c
return n * fac_naive(n - 1);   /* NOT tail — multiply after call */
return fac_tail(n - 1, acc * n); /* IS tail — return callee's result as-is */
```

| Form | Work after recursive call? | TCO-eligible in principle? |
| ---- | -------------------------- | -------------------------- |
| `n * fac(n-1)` | Yes — multiply | No |
| `fac_tail(n-1, acc*n)` | No — result returned unchanged | Yes |
| Logging / `printf` after call | Yes | Blocks TCO |

If you add logging after the call, you may block the optimization you came to
observe.

## What TCO *can* look like in asm

In asm, a successful tail-call optimization often shows:

- Argument setup for the next invocation (`mov` into `%rdi` / `%rsi`, …),
- Then `jmp fac_tail` (or a local label at the top of the function),
- **No** `call` + pending multiply.

The function becomes iterative in machine terms while still reading as recursion
in C. That is why deep "recursion" can suddenly stop growing the stack at `-O2`.

### Recognition rules

| Pattern | Reading |
| ------- | ------- |
| `call fac_tail` then `ret` with nothing in between that uses the result except
  returning it | still a call — maybe no TCO at this level |
| `jmp fac_tail` or `jmp .Ltop` after arg updates | classic TCO / self-tail |
| Loop: `imul` + `sub` + `jne .L2` with **no** recursive `call` | compiler rewrote recursion into iteration (also kills stack growth) |

**Rejected wrong reading:** "any `jmp` in the function proves TCO" — jumps also
implement loops and control flow inside the body. Context: is it re-entering with
updated args as the primary recursive step, or just a local branch?

**Rejected wrong reading:** "tail recursion always shows as `jmp` even at
`-O0`." `-O0` favors straightforward code generation — expect `call`.

## Why naive factorial resists *as a tail call*

`return n * fac(n - 1);` must keep `n` alive across the call, then multiply. That
post-call work means the call is not a tail call. GCC may still optimize in other
ways (inlining for small `n`, converting the whole function into a loop at
`-O2`, etc.), but that loop rewrite is *not* the same claim as "TCO happened."

On modern GCC `-O2`, you may see **both** `fac_naive` and `fac_tail` become
loops with `imulq` and no recursive `call`. That is still a win for stack depth —
record what you actually got:

| Observation | Fair conclusion |
| ----------- | --------------- |
| `fac_tail` is a self-`jmp` with arg updates | classic TCO evidence |
| Both became loops, no `call` | `-O2` rewrote recursion; stack won't nest |
| `fac_naive` still has `call` at `-O2` | post-call multiply survived |

Do not force a "naive still calls" story if your listing shows a loop — cite the
instructions you have.

## Worked example

**The situation.** Both functions compute factorial for modest `n`. You inspect
`-O0` and `-O2` listings.

**Step 1 — `-O0` both.** Expect honest `call` instructions in each. Search
`fac_naive:` / `fac_tail:` and list every `call`.

**Step 2 — `-O2` `fac_tail`.** Look for `jmp` to itself / self-label, a tight
loop, or absence of recursive `call`. Quote the instructions.

**Step 3 — `-O2` `fac_naive`.** Find recursive `call`, or a loop rewrite. Explain
using post-call work in the *C* source even if the machine code was rewritten.

**Step 4 — optional gdb contrast.** Deep `fac_naive` at `-O0` grows `%rsp` (lesson
13); a true non-nesting `-O2` build need not. Measure only if the listing already
suggests no nested `call`.


## Citation phrases that stay honest

Use sentences like these in your notes:

- "At `-O2`, `fac_tail` contains no `call`; it has `jmp .L11` / a mul loop at
  addresses …"
- "At `-O2`, `fac_naive` was rewritten to a loop with `imulq`; that is not
  classical TCO, but it also avoids nested frames."
- "At `-O0`, both still `call` themselves at …"

Avoid: "TCO always happened because `-O2`." Demand instruction evidence.


## What to open after each build

| Build | File | Questions |
| ----- | ---- | --------- |
| `-O0` | `prog-O0.s` | Does `fac_naive` `call` itself? Does `fac_tail`? |
| `-O2` | `prog-O2.s` | Any recursive `call` left? `jmp`? Loop with `imul`? |
| `-O2` | `prog.lst` | Confirm symbols `<fac_naive>:` / `<fac_tail>:` still exist |

If a function disappeared entirely (inlined into `main`), say so — that is also
an optimization outcome, just not the TCO fingerprint.


## Optional stack check (only after the listing)

If `-O2` `fac_tail` shows no nested `call`, a quick gdb session with a large `n`
should show `%rsp` staying nearly flat across what C still *looks* like
recursion. If you still see deep nesting, your listing and your binary disagree
— rebuild cleanly and re-disassemble.


## Rejected wrong reading (language rules)

"**C guarantees tail-call optimization, so my recursive factorial cannot overflow
the stack if I write it in accumulator style.**" C does not guarantee TCO. GCC
may optimize; other compilers may not. Always check the `-O2` listing for *this*
toolchain.

## Distinctions worth keeping straight

- **Tail position vs syntactic recursion** — placement of work matters.
- **`call` vs `jmp` reuse vs loop rewrite** — three different machine stories.
- **`-O0` clarity vs `-O2` rewrite** — same C, different machines.
- **Algorithmic rewrite vs guaranteed TCO** — C does not require TCO; GCC may
  optimize more aggressively than "just TCO."
- **Stack growth vs presence of the word "recursive" in C** — `-O2` can break the
  link.

## Check yourself

1. Why is `n * fac(n-1)` not a tail call in the C abstract machine?
2. What asm evidence suggests `fac_tail` was optimized into iterative form?
3. Why might `-O0` still show `call` in `fac_tail`?
4. If `-O2` turns `fac_naive` into a loop with no `call`, did TCO happen — or
   something else?
5. How do you avoid mistaking a local `jmp .Ldone` for TCO?
6. What files must you keep if `make O=2 asm` overwrites `prog.s`?

## Key takeaways

- Tail calls may become jumps that reuse a frame under optimization — or the
  whole recursion may become a loop.
- Work after a recursive call blocks *tail*-call status in C, even when `-O2`
  rewrites the function.
- Prove it from `-O2` asm / `objdump`, not from hope — cite `call` / `jmp` /
  loop instructions.
- C does not guarantee TCO; observe what GCC actually did.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info gcc` — optimization notes after practice

Now open `TASK.md` and do the practice.
