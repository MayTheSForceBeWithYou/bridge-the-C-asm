# 28 — Recursive factorial in pure asm

Recursion is a calling convention stress test: each invocation needs its own
saved state, a correct base case, and a `call` that comes back. This stretch
lesson makes you build `fac(n)` with manual frames so the stack story from
Track A becomes something you emit, not only something you read.

## What this lesson asks of you

Implement `long fac(long n)` with `fac(0) == 1` and `n >= 0` in tests, by calling
`fac` from itself in asm. Return the product in `%rax`. Save whatever must
survive across the recursive call.

## Recursion as a protocol

## Multiply forms (AT&T)

A convenient two-operand form is `imulq %reg, %rax` (or the reverse destination
you choose): it multiplies the destination by the source and leaves the product
in the destination. Know which register holds `n` and which holds `fac(n-1)`
before you issue it. Clobbering the saved `n` with the product too early makes
debugging painful — finish the multiply into `%rax`, then tear down the frame.

Depth for harness values is tiny; correctness of save/restore matters more than
micro-optimization.


For `n > 0`, mathematically `fac(n) = n * fac(n-1)`. In asm that means:

1. Test for the base case (`n == 0` → move `1` into `%rax`, return).
2. Preserve `n` somewhere the recursive call will not destroy (stack slot or
   callee-saved register).
3. Put `n-1` in `%rdi`, align `%rsp`, `call fac`.
4. Multiply the returned `%rax` by the saved `n`.
5. Epilogue and `ret`.

The recursive `call` re-enters the same symbol with a new stack frame. Forgetting
to save `n` is the classic bug: after the call, `%rdi` is not guaranteed to
still hold the original argument.

## Frames and alignment again

Each level should follow the same outbound-call rules as exercise 27. If you
`push %rbp` and spill `n` at `-8(%rbp)`, subtract enough that `%rsp` is aligned
before `call fac`. Depth for test values is small, but wrong alignment still
faults if a deeper path hits a fussy callee — here the callee is yourself, and
your own code might tolerate misalignment until something else does not. Build
the habit anyway.

Tail-call elimination (exercise 14) can turn factorial into a loop; this
exercise wants real recursive `call`s so you feel frame growth.

## Worked example

**The situation.** Stub `fac.s` must pass several `n` in the harness.

**Step 1 — base case first.** Compare `%rdi` to zero; on equality load `1` into
`%rax` and jump to a shared epilogue (or `ret` carefully if nothing was
pushed). The rejected wrong reading is: "base case returns 0." Factorial of
zero is 1.

**Step 2 — save `n`, call with `n-1`.** Spill `%rdi`, decrement a copy into
`%rdi`, `call fac`. The rejected wrong reading is: "`%rdi` survives the call
because I am calling myself." ABI caller-saved rules do not special-case
recursion.

**Step 3 — multiply and return.** Reload saved `n`, multiply by `%rax`
(`imulq`), ensure the product is in `%rax`, restore frame, `ret`. Check
`fac(5) == 120` mentally before trusting `make run`.

## Distinctions worth keeping straight

- **Base value 1 vs 0** — empty product is 1.
- **Saving `n` vs hoping registers last** — recursion is a call; treat it like
  one.
- **Recursive `call` vs loop** — both can compute factorial; only `call` grows
  frames per level.
- **`imul` result location** — know which form leaves the product where you
  need it (`imulq reg, reg` grows the destination).

## Check yourself

1. What does `fac(0)` return, and why?
2. Which values must be saved across the recursive `call`?
3. What goes in `%rdi` for the recursive invocation?
4. Why is "I called myself, so caller-saved rules relax" false?

## Key takeaways

- Recursive asm is ordinary `call` plus preserved per-frame state.
- Base case `fac(0) = 1`; missing it poisons every larger `n`.
- Save `n` before calling `fac(n-1)`; multiply after return.
- Alignment and ABI rules apply equally to self-calls.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Compare to exercises 13–14 (C recursion / tail calls)

Now open `TASK.md` and do the practice.
