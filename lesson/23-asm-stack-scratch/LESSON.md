# 23 — Stack scratch and 16-byte alignment

Registers are not enough once you need memory the caller cannot see. Locals live
in a stack frame you carve from `%rsp`, and the ABI demands that `%rsp` be
16-byte aligned *before* any `call`. This lesson connects prologue arithmetic to
that rule so your scratch space does not accidentally break the next call.

## What this lesson asks of you

Implement `long scratch_sum(long a, long b)` by storing both arguments into
stack locals, reloading them, and returning the sum. Even if you never call a
helper, practice a prologue that keeps alignment coherent the way real framed
code does.

## Building a frame (AT&T shape)

A common `-O0` style prologue:

1. `push %rbp` — saves the caller's frame pointer and changes `%rsp` by 8.
2. `mov %rsp, %rbp` — anchors locals at stable offsets from `%rbp`.
3. `sub $N, %rsp` — reserves `N` bytes for locals (and padding).

Arguments still arrive in `%rdi` and `%rsi`. You spill them to memory with
stores such as `movq %rdi, -8(%rbp)`, then reload with `movq` before adding
into `%rax`. The epilogue restores `%rsp` / `%rbp` (often `leave`, or
`mov %rbp, %rsp` then `pop %rbp`) and `ret`.

## Why 16-byte alignment bites

The ABI requires `%rsp ≡ 0 (mod 16)` at the moment a `call` instruction
executes. `call` itself pushes an 8-byte return address, so on entry to the
callee `%rsp ≡ 8 (mod 16)`.

Then `push %rbp` subtracts another 8, so `%rsp` is 16-byte aligned again at
that instant. If you `sub` an amount that is not a multiple of 16 afterward,
you re-break alignment before a nested `call`. Compilers therefore round the
frame size so that after the prologue, `%rsp` stays correctly aligned for
outgoing calls.

Even without a nested call in this exercise, accounting for the `push` is the
habit that prevents mysterious crashes inside libc later (exercise 72).

## Locals as addressed memory

A "local quad" is eight bytes on the stack. Two locals need at least 16 bytes
of reservation, plus any padding your alignment story requires. Addressing from
`%rbp` with negative offsets is the readable pattern matching Track A `-O0`
output; addressing from `%rsp` also works if you track the offsets carefully.

The point of spilling `a` and `b` even though they already sit in registers is
pedagogical: you practice stores and loads through the frame, not the fastest
code.

## Worked example

**The situation.** Stub `scratch.s` must become a framed function the harness
calls as `scratch_sum`.

**Step 1 — write a prologue and reserve space.** After `push %rbp` /
`mov %rsp, %rbp`, subtract enough for two quads (and padding if you plan a
`call`). The rejected wrong reading is: "I can `sub $8` twice without thinking
about total frame alignment." Think in the final `%rsp` modulo 16.

**Step 2 — spill, reload, add.** Store `%rdi` and `%rsi` into distinct slots,
load them back into registers, add, leave the sum in `%rax`. The rejected wrong
reading is: "returning the value left in `%rdi` is fine." The C caller reads
`%rax`.

**Step 3 — epilogue and harness.** Restore the caller's `%rbp`/`%rsp` and
`ret`. `make && make run` should print `ok`. The rejected wrong reading is:
"if the sum is right I can skip restoring `%rbp`." Your caller may still be
using `%rbp`; trashing it fails later, not always inside your function.

## Distinctions worth keeping straight

- **Alignment at `call` vs alignment on entry** — entry has the return address
  already pushed (`%rsp ≡ 8 mod 16`).
- **`%rbp` frame vs raw `%rsp` addressing** — same memory; different stability
  when you push more.
- **Reserving space vs pushing args** — outgoing stack args (rare for the first
  six integers) are separate from local scratch.
- **Needing a frame vs being required to use callee-saved regs** — orthogonal
  concerns.

## Check yourself

1. After `call` enters your function, what is `%rsp` mod 16 before your first
   instruction?
2. What does `push %rbp` do to that congruence?
3. Why might `sub $8, %rsp` after the push be a bad idea before a nested
   `call`?
4. Where does the returned sum belong?

## Key takeaways

- Stack locals are bytes you subtract from `%rsp` (often addressed via `%rbp`).
- ABI rule: `%rsp` 16-byte aligned at each `call`; account for the pushed return
  address and `push %rbp`.
- Spill/reload practice is about memory traffic through the frame, not speed.
- Broken alignment often dies inside callees, not at your `sub` line.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Prologue patterns from Track A `gcc -S -O0` `.s` files

Now open `TASK.md` and do the practice.
