# 14 — Tail-call optimization?

A call is in *tail position* when the caller immediately returns its result —
nothing remains to do after the callee comes back. Compilers may turn such a
call into a `jmp`, reusing the current frame instead of nesting another. Classic
`n * fac(n-1)` is *not* tail recursion because the multiply happens after the
call. An accumulator helper `fac_tail(n, acc)` can be. By the end of this lesson
you should see whether `-O2` rewrote your tail version into a loop-like jump and
explain why the naive version still calls.

## What this lesson asks of you

Implement `fac_naive` and `fac_tail`, compare `make asm` versus `make O=2 asm`,
and confirm with `objdump -d`. `TASK.md` is the practice.

## What TCO looks like

In asm, a successful tail-call optimization often shows:

- Argument setup for the next invocation,
- Then `jmp fac_tail` (or a local label at the top of the function),
- **No** `call` + pending multiply.

The function becomes iterative in machine terms while still reading as recursion
in C. That is why deep "recursion" can suddenly stop growing the stack at `-O2`.

## Why naive factorial resists

`return n * fac(n - 1);` must keep `n` alive across the call, then multiply. That
post-call work means the call is not a tail call. GCC may still optimize in other
ways (inlining for small `n`, etc.), but you should not expect a pure jump that
reuses the frame for the recursive request unless the compiler rewrites the
algorithm.

## Accumulator pattern in C

`fac_tail(n, acc)` typically does `if (n==0) return acc; return fac_tail(n-1,
acc*n);` — the recursive call's result is returned unchanged. That "returned
unchanged" property is what makes the call eligible for TCO. If you add logging
after the call, you may block the optimization you came to observe.

## Worked example

**The situation.** Both functions compute factorial for modest `n`. You inspect
`-O0` and `-O2` listings.

**Step 1 — `-O0` both.** Expect honest `call` instructions in each. The rejected
wrong reading is: "tail recursion always shows as `jmp` even at `-O0`." `-O0`
favors straightforward code generation.

**Step 2 — `-O2` `fac_tail`.** Look for `jmp` to itself / self-label and absence
of recursive `call`. The rejected wrong reading is: "any `jmp` in the function
proves TCO" — jumps also implement loops and control flow inside the body.
Context: is it re-entering with updated args as the primary recursive step?

**Step 3 — `-O2` `fac_naive`.** Find the recursive `call` still present, or a
different rewrite that is *not* simply TCO. Explain using post-call work.

**Step 4 — optional gdb contrast.** Deep `fac_naive` at `-O0` grows `%rsp`; a
true TCO build of `fac_tail` need not. Measure only if the listing already
suggests TCO.

## Distinctions worth keeping straight

- **Tail position vs syntactic recursion** — placement of work matters.
- **`call` vs `jmp` reuse** — nested frame versus reused frame.
- **`-O0` clarity vs `-O2` rewrite** — same C, different machines.
- **Algorithmic rewrite vs guaranteed TCO** — C does not require TCO; GCC may.

## Check yourself

1. Why is `n * fac(n-1)` not a tail call?
2. What asm evidence suggests `fac_tail` was optimized into iterative form?
3. Why might `-O0` still show `call` in `fac_tail`?
4. If `-O2` still calls in `fac_naive`, does that mean optimization "failed"?

## Key takeaways

- Tail calls may become jumps that reuse a frame under optimization.
- Work after a recursive call blocks TCO.
- Prove it from `-O2` asm / `objdump`, not from hope.
- C does not guarantee TCO; observe what GCC actually did.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`
- `info gcc` — optimization notes after practice

Now open `TASK.md` and do the practice.
