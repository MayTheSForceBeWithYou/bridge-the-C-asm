# 28 — Recursive factorial in pure asm

Recursion is a calling convention stress test: each invocation needs its own
saved state, a correct base case, and a `call` that comes back. This stretch
lesson makes you build `fac(n)` with manual frames so the stack story from
Track A becomes something you emit, not only something you read. By the end you
should name what must be saved across the recursive call, keep alignment, and
refuse the myth that "calling myself relaxes caller-saved rules."

## What this lesson asks of you

Implement `long fac(long n)` with `fac(0) == 1` and `n >= 0` in tests, by calling
`fac` from itself in asm. Return the product in `%rax`. Save whatever must
survive across the recursive call.

## Recursion as a protocol

For `n > 0`, mathematically `fac(n) = n * fac(n-1)`. In asm that means a
checklist — treat each row as a recognition rule:

| Step | Action | Why |
| ---- | ------ | --- |
| 1 | Test base: `n == 0` → `%rax = 1`, return | Empty product is 1 |
| 2 | Preserve `n` | Recursive call clobbers caller-saved regs including `%rdi` |
| 3 | Put `n-1` in `%rdi` | Inbound arg for the child frame |
| 4 | Align `%rsp`, `call fac` | Same outbound rules as exercise 27 |
| 5 | Multiply returned `%rax` by saved `n` | Finish the product |
| 6 | Epilogue, `ret` | Restore caller's frame |

The recursive `call` re-enters the same symbol with a new stack frame.
Forgetting to save `n` is the classic bug: after the call, `%rdi` is not
guaranteed to still hold the original argument.

**Rejected wrong reading:** "`%rdi` survives the call because I am calling
myself." ABI caller-saved rules do not special-case recursion.

**Rejected wrong reading:** "Base case returns 0." Factorial of zero is 1;
returning 0 poisons every larger `n`.

## Multiply forms (AT&T)

A convenient two-operand form is `imulq %reg, %rax`: it multiplies `%rax` by
`%reg` and leaves the product in `%rax`. Know which register holds saved `n` and
which holds `fac(n-1)` before you issue it.

| Form | Result location |
| ---- | --------------- |
| `imulq %rbx, %rax` | product in `%rax` |
| Other `imul` variants | check width and implicit `%rdx:%rax` forms if you use them |

Clobbering the saved `n` with the product too early makes debugging painful —
finish the multiply into `%rax`, then tear down the frame.

Depth for harness values is tiny; correctness of save/restore matters more than
micro-optimization.

## Frames and alignment again

Each level should follow the same outbound-call rules as exercise 27. If you
`push %rbp` and spill `n` at `-8(%rbp)`, subtract enough that `%rsp` is aligned
before `call fac`. Decode the slot the same way as exercise 23:

| Operand | Meaning |
| ------- | ------- |
| `-8(%rbp)` | address of saved `n` |
| `movq %rdi, -8(%rbp)` | spill inbound `n` |
| `movq -8(%rbp), %rcx` | reload after the child returns (register choice yours) |

Tail-call elimination (exercise 14) can turn factorial into a loop; this
exercise wants real recursive `call`s so you feel frame growth.

### What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | several `n`; expect `fac(5) == 120` among others |
| `objdump -d` | search `<fac>:`; you should see `call … <fac>` |
| gdb `bt` mid-recursion (optional) | multiple `fac` frames |

## How to navigate the listing

1. Search `<fac>:`.
2. Find the base-case path: a `cmp`/`test` against zero and a move of `$1` into
   `%rax`.
3. Find `call` whose target hint is `<fac>` — that is the recursive edge. Left
   column = call site address; operand = callee (same symbol).
4. Near the call, find a spill/reload of `n` (memory operand or callee-saved
   reg). If you cannot find one, you probably have the classic bug waiting.
5. Find `imul` (or equivalent) after the call.

**Rejected wrong reading:** "The first `call` in the file is my recursion."
Anchor on `<fac>:` first; CRT and harness `main` may call other things earlier.

## Worked example

**The situation.** Stub `fac.s` must pass several `n` in the harness.

**Step 1 — base case first.** Compare `%rdi` to zero; on equality load `1` into
`%rax` and jump to a shared epilogue (or `ret` carefully if nothing was
pushed).

**Step 2 — save `n`, call with `n-1`.** Spill `%rdi`, decrement a copy into
`%rdi`, ensure alignment, `call fac`.

**Step 3 — multiply and return.** Reload saved `n`, `imulq` into `%rax`,
restore frame, `ret`. Check `fac(0)`, `fac(1)`, `fac(5)` mentally before
trusting `make run`.

**Step 4 — read the recursive `call` in `objdump`.** Confirm it is inside
`<fac>:` and that a save of `n` surrounds it. The rejected wrong reading is
optimizing away the save because "the harness only tests small n" — wrong saves
fail as soon as the child clobbers `%rdi`.

## Distinctions worth keeping straight

- **Base value 1 vs 0** — empty product is 1.
- **Saving `n` vs hoping registers last** — recursion is a call; treat it like
  one.
- **Recursive `call` vs loop** — both can compute factorial; only `call` grows
  frames per level.
- **`imul` result location** — know which form leaves the product where you
  need it.
- **Call site address vs `<fac>` target** — same symbol, different questions
  (where / who).

## Check yourself

1. What does `fac(0)` return, and why?
2. Which values must be saved across the recursive `call`?
3. What goes in `%rdi` for the recursive invocation?
4. Why is "I called myself, so caller-saved rules relax" false?
5. On a `call … <fac>` line inside your listing, which token is the call
   instruction's address?
6. Where must the final product be at `ret`?

## Key takeaways

- Recursive asm is ordinary `call` plus preserved per-frame state.
- Base case `fac(0) = 1`; missing it poisons every larger `n`.
- Save `n` before calling `fac(n-1)`; multiply after return.
- Alignment and ABI rules apply equally to self-calls.
- Find recursion with `<fac>:` and a `call` back to `<fac>` — not the first
  `call` in the file.


## Watching frames grow (optional)

Under gdb, `break fac`, `run` from the harness, then `continue` a few times and
`bt`. You should see multiple `fac` frames for `n > 1`. If you only ever see
one frame while computing `fac(5)`, you may have accidentally written a loop
(or a tail call) instead of a recursive `call`. This exercise wants the
recursive shape on purpose — exercise 14 is where tail-call elimination becomes
the topic.


## Minimal spill sketch (shape only)

One honest layout after a framed prologue:

| Slot / reg | Holds |
| ---------- | ----- |
| `-8(%rbp)` | saved `n` |
| `%rdi` before `call` | `n-1` |
| `%rax` after `call` | `fac(n-1)` |
| `%rax` after `imul` | `n * fac(n-1)` |

This is not the only layout and not a paste-ready solution — it is a checklist
you can tick while reading your own listing. If a row is missing in the
disassembly, that row is probably your bug.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Compare to exercises 13–14 (C recursion / tail calls)
- `man 1 objdump`, `man 1 gdb` — optional frame inspection

Now open `TASK.md` and do the practice.
