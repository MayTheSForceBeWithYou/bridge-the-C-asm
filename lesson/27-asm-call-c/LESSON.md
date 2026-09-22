# 27 — Calling C from assembly

So far C called your asm. The other direction matters just as much: your asm
sets up arguments, aligns the stack, `call`s a C function, and treats `%rax` as
the result. This lesson practices outbound calls with callee-saved discipline
so you do not corrupt the C world you jump into. By the end you should state
`%rsp` mod 16 on entry and at `call`, and refuse to overwrite `%rax` after the
callee returns.

## What this lesson asks of you

C already provides `long c_triple(long x)` in the harness. You implement
`long apply_triple(long x)` in asm: it must `call c_triple` and return that
function's result. Alignment before `call` and correct arg setup are the real
tests.

## Outbound call checklist

Before `call c_triple`:

| Step | Requirement | Recognition |
| ---- | ----------- | ----------- |
| 1. Argument | `x` in `%rdi` | May already be there on entry if you did not clobber it |
| 2. Alignment | `%rsp ≡ 0 (mod 16)` at the `call` | On entry you had 8; fix with one 8-byte push or `sub $8` |
| 3. Call | `call c_triple` | Linker resolves the harness symbol |
| 4. Result | `3*x` in `%rax` | Do not overwrite before your `ret` |
| 5. Epilogue | pop what you pushed | Restore caller's view of callee-saved regs |

## What `call` does to the stack

`call` pushes the return address (8 bytes) and jumps. Alignment is measured
*at* the `call` instruction: after it executes, the callee sees
`%rsp ≡ 8 (mod 16)` again. Your job as caller is to ensure the push starts from
a 16-byte-aligned `%rsp`.

Walk the numbers for this function:

| Moment | `%rsp` mod 16 |
| ------ | ------------- |
| First insn of `apply_triple` | 8 (harness's `call` already pushed) |
| After one `push %rbp` (or `sub $8`) | 0 |
| At your `call c_triple` | must be 0 |
| After `c_triple` returns | restored to your pre-call value |

When `c_triple` returns, anything you pushed for alignment or saves must still
be popped in reverse order before your own `ret`.

**Rejected wrong reading:** "Alignment only matters for SIMD." On this ABI it
matters for every `call`; SSE spills inside libc assume it.

**Rejected wrong reading:** "If I `push %rbx` alone and never adjust further,
alignment is still wrong… wait." On entry you are at 8; one push brings you to
0 — so a *single* 8-byte push *does* fix alignment. An *odd* number of pushes
from a state that was already aligned would break it; count carefully.

## Callee-saved if you need them

If you must keep `x` across the call and you choose a callee-saved register
(`%rbx`, …), push it in the prologue and pop in the epilogue. If you keep `x` in
a caller-saved register, assume `c_triple` destroyed it — save it on the stack
yourself before the call. Reading Track A `-O0` `.s` around calls shows which
registers GCC saves; mirror that judgment.

You do not need a full frame pointer, but many solutions use `push %rbp` /
`mov %rsp, %rbp` because the push also fixes alignment in one stroke.

### What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | harness checks `apply_triple(k) == 3*k` |
| `objdump -d` | search `<apply_triple>:`; find `call … <c_triple>` |

## How to navigate the listing

1. Search `<apply_triple>:`.
2. Note prologue pushes / `sub` — that is your alignment fix.
3. Find `call` toward `c_triple` (may show as `call … <c_triple>` or similar).
4. On that `call` line, left column = where *your* call instruction lives; the
   operand / hint = where it goes.
5. After the call, confirm you do not see a `mov %rdi, %rax` that would trash
   the result.

Example shape:

```text
    1190:	e8 bb ff ff ff       	call   1150 <c_triple>
    1195:	c9                   	leave
    1196:	c3                   	ret
```

| Token | Meaning |
| ----- | ------- |
| `1190:` | address of your `call` |
| `e8 …` | relative call encoding |
| `<c_triple>` | callee symbol hint |
| No mov into `%rax` after | result already correct |

**Rejected wrong reading:** "I should `mov %rdi, %rax` after the call." The
result is already in `%rax`; `%rdi` may be garbage now.

## Worked example

**The situation.** Stub `apply_triple.s` must call through to `c_triple`.

**Step 1 — align then call.** On entry, push `%rbp` (or subtract 8), keep or
reload `%rdi`, `call c_triple`. Mentally check: entry mod 16 was 8; after push
it is 0.

**Step 2 — do not trash `%rax` after the call.** Add no "helpful" zeroing.

**Step 3 — epilogue.** Pop what you pushed, `ret`. Harness prints `ok` when
`apply_triple(k) == 3*k` for its cases.

**Step 4 — deliberate misalignment (optional observation).** Comment out the
alignment push, rebuild, run. If it faults, note *which* frame appears in a
backtrace — often inside the C callee or libc, not on your `sub` line. That is
evidence for the alignment story (exercise 72 revisits this harder).

## Distinctions worth keeping straight

- **Inbound ABI vs outbound ABI** — same registers; you are the caller now.
- **`call` vs `jmp`** — `call` pushes a return address; `jmp` does not.
- **Your saves vs the callee's saves** — you preserve callee-saved for *your*
  caller; the callee preserves them for you.
- **Instruction address of `call` vs callee address** — left column vs operand.
- **Symbol `c_triple`** — defined in C, declared by use in asm; the linker
  stitches it.

## Check yourself

1. What is `%rsp` mod 16 on entry to `apply_triple`, and what must it be at
   `call`?
2. Which register must hold `x` when `c_triple` starts?
3. After `c_triple` returns, where is `3*x`?
4. If you `push %rbx` alone from entry alignment 8, is alignment correct at
   `call`?
5. On a `call … <c_triple>` line, which token is the address of the call
   instruction?
6. What search string finds your wrapper rather than `c_triple` itself?

## Key takeaways

- Calling C from asm: set arg regs, align `%rsp` to 16, `call`, take `%rax`.
- One 8-byte push often fixes the entry alignment skew from the return address.
- Do not clobber the callee's return value on the way out.
- Callee-saved registers need save/restore only if you use them across the call.
- Faults from bad alignment often appear inside the callee — use that clue.


## Reading the harness contract

The C side exports `c_triple` and calls `apply_triple`. You never redefine
`c_triple` in asm. If the linker complains about an undefined `c_triple`, your
`call` spelling or the build graph is wrong — not your arithmetic. If the
linker is happy but results are wrong, look at alignment and `%rax` preservation
first, then argument setup.

Optional check: `nm` on the linked binary should show both `c_triple` and
`apply_triple` as defined text symbols (`T`). Missing `c_triple` means the
harness object did not participate in the link.


## PLT hint versus your symbol

In `objdump -d`, `call c_triple` may appear with a bare `<c_triple>` if it is
defined in the same linkage unit, unlike libc calls that show `@plt`. Either
way, the left column is still *your* call instruction's address. Do not hunt
for `@plt` as proof the call worked — hunt for the correct symbol name and a
preceding alignment fix.

## Lookup (not the lesson)

- `man 1 as`
- Caller/callee-saved patterns from Track A `-O0` `.s` (no ABI PDF)
- `man 1 objdump`

Now open `TASK.md` and do the practice.
