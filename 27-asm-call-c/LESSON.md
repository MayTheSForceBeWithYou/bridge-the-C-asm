# 27 — Calling C from assembly
<!-- concepts: c.p5.abi.call, c.p5.asm.function -->

So far C called your asm. The other direction matters just as much: your asm
sets up arguments, aligns the stack, `call`s a C function, and treats `%rax` as
the result. This lesson practices outbound calls with callee-saved discipline
so you do not corrupt the C world you jump into.

## What this lesson asks of you

C already provides `long c_triple(long x)` in the harness. You implement
`long apply_triple(long x)` in asm: it must `call c_triple` and return that
function's result. Alignment before `call` and correct arg setup are the real
tests.

## Outbound call checklist

## What `call` does to the stack

`call` pushes the return address (8 bytes) and jumps. That is why alignment is
measured *at* the `call` instruction: after it executes, the callee sees
`%rsp ≡ 8 (mod 16)` again. Your job as caller is to ensure the push starts from
a 16-byte-aligned `%rsp`.

When `c_triple` returns, `%rsp` is restored to your pre-call value (assuming the
callee obeyed the ABI). Anything you pushed for alignment or saves must still be
popped in reverse order before your own `ret`.


Before `call c_triple`:

1. **Argument:** place `x` in `%rdi` (it may already be there on entry to
   `apply_triple` — if you did not clobber `%rdi`, you are done).
2. **Alignment:** `%rsp ≡ 0 (mod 16)` at the `call` instruction. On entry to
   `apply_triple`, `%rsp ≡ 8 (mod 16)` because of the return address. A single
   `push` (often `%rbp` or a callee-saved scratch) restores alignment; a
   `sub $8, %rsp` can do the same. If you push an odd number of quads without
   compensating, libc or C code may crash inside the callee.
3. **Call:** `call c_triple` (the linker resolves the symbol from the harness
   object).
4. **Return:** `c_triple` leaves `3*x` in `%rax`; that is already your return
   value if you do not overwrite it. Restore any saved registers, then `ret`.

## Callee-saved if you need them

If you must keep `x` across the call and you choose a callee-saved register
(`%rbx`, …), push it in the prologue and pop in the epilogue. If you keep `x` in
a caller-saved register, assume `c_triple` destroyed it — save it on the stack
yourself before the call. Reading Track A `-O0` `.s` around calls shows which
registers GCC saves; mirror that judgment.

You do not need a full frame pointer, but many solutions use `push %rbp` /
`mov %rsp, %rbp` because the push also fixes alignment in one stroke.

## Worked example

**The situation.** Stub `apply_triple.s` must call through to `c_triple`.

**Step 1 — align then call.** On entry, push `%rbp` (or subtract 8), keep or
reload `%rdi`, `call c_triple`. The rejected wrong reading is: "alignment only
matters for SIMD." On this ABI it matters for every `call`; SSE spills inside
libc assume it.

**Step 2 — do not trash `%rax` after the call.** Add no "helpful" zeroing.
The rejected wrong reading is: "I should `mov %rdi, %rax` after the call." The
result is already in `%rax`; `%rdi` may be garbage now.

**Step 3 — epilogue.** Pop what you pushed, `ret`. Harness prints `ok` when
`apply_triple(k) == 3*k` for its cases.

## Distinctions worth keeping straight

- **Inbound ABI vs outbound ABI** — same registers; you are the caller now.
- **`call` vs `jmp`** — `call` pushes a return address; `jmp` does not.
- **Your saves vs the callee's saves** — you preserve callee-saved for *your*
  caller; the callee preserves them for you.
- **Symbol `c_triple`** — defined in C, declared by use in asm; the linker
  stitches it.

## Check yourself

1. What is `%rsp` mod 16 on entry to `apply_triple`, and what must it be at
   `call`?
2. Which register must hold `x` when `c_triple` starts?
3. After `c_triple` returns, where is `3*x`?
4. If you `push %rbx` alone and never adjust further, is alignment correct at
   `call`?

## Key takeaways

- Calling C from asm: set arg regs, align `%rsp` to 16, `call`, take `%rax`.
- One 8-byte push often fixes the entry alignment skew from the return address.
- Do not clobber the callee's return value on the way out.
- Callee-saved registers need save/restore only if you use them across the call.

## Lookup (not the lesson)

- `man 1 as`
- Caller/callee-saved patterns from Track A `-O0` `.s` (no ABI PDF)

Now open `TASK.md` and do the practice.
