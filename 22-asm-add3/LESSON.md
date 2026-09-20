# 22 — Hand-written `add3` under the C ABI
<!-- concepts: c.p5.abi.argregs, c.p5.asm.function -->

C and assembly meet at a calling convention: where arguments land, where the
return value must appear, and which registers you may freely overwrite. This
lesson makes you implement a three-argument integer function the way System V
AMD64 expects, so a C harness can call your `.s` without glue.

## What this lesson asks of you

Implement `long add3(long a, long b, long c)` in GNU as AT&T so the sum returns
in `%rax`. You should already have seen the register story from Track A
(arguments and returns); here you *author* it. No callee-saved traffic is
required if you only touch caller-saved registers.

## Integer argument and return registers

For the System V AMD64 ABI used on Linux x86-64, the first integer/pointer
arguments arrive in `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, `%r9`. The function
return value for integers and pointers goes in `%rax`.

So for `add3(a, b, c)`:

- `a` is already in `%rdi`;
- `b` in `%rsi`;
- `c` in `%rdx`;
- you leave `a+b+c` in `%rax` before `ret`.

You discover this from `gcc -S` on a tiny C `add3`, not from an ABI PDF. The
compiler's `.s` is the local ground truth this track trusts.

## Caller-saved versus callee-saved (just enough)

If you only use registers the caller already treats as scratch (`%rax`, `%rdi`,
`%rsi`, `%rdx`, and friends), you can compute and `ret` with no `push`/`pop`.
Callee-saved registers (`%rbx`, `%rbp`, `%r12`–`%r15`) must be preserved if you
use them — save before, restore before `ret`. For a three-register add you
should not need them.

## Shape of a minimal AT&T function

A global function needs a visible symbol (`.globl add3` / `.global add3`), a
label `add3:`, instructions that honor the register contract, and `ret`. You
are not writing `_start`; the C harness and CRT still own process startup. Your
job is only the callee side of one call.

AT&T syntax writes source then destination: `addq %rsi, %rdi` adds `%rsi` into
`%rdi`. Width suffixes (`q` for 64-bit) match the `long` in the C prototype on
this LP64 platform.

## Worked example

**The situation.** The stub `add3.s` exports `add3` but only `ret`s, so the
harness sees a garbage return and prints `FAIL`.

**Step 1 — name the registers from a C reference.** Compile a one-line C
`add3` with `gcc -S -O0` (or revisit exercise 04–05 artifacts). Confirm the
three incoming registers and that the `add`/`lea` chain ends with a value in
`%rax`. The rejected wrong reading is: "arguments are on the stack like i386."
On this ABI the first six integer args are in registers.

**Step 2 — implement only the arithmetic.** Move or add so `%rax` holds the
sum, then `ret`. The rejected wrong reading is: "I must push `%rbp` and build a
frame." A leaf that needs no locals may skip a frame; the ABI does not require
a frame pointer.

**Step 3 — run the harness.** `make && make run` expects cases like
`add3(1,2,3) == 6`. If you swapped two argument registers, one case may pass by
luck while another fails — read failures as ABI mistakes, not "math bugs."

## Distinctions worth keeping straight

- **C prototype vs asm symbol** — same name, same register contract; the
  assembler does not check types.
- **Return in `%rax` vs "the last register I added into"** — if you left the
  sum only in `%rdi`, C still reads `%rax`.
- **Leaf function vs framed function** — frames are for locals and debug
  convenience; they are not mandatory for every callee.
- **Your `.s` vs freestanding `_start`** — this exercise still links with CRT
  and a C `main` in the harness.

## Check yourself

1. In which three registers do `a`, `b`, and `c` arrive?
2. Where must the sum be when `ret` executes?
3. If you used `%rbx` as a temporary, what else must you do before returning?
4. Why can this function omit `push %rbp` and still be ABI-correct?

## Key takeaways

- Linux x86-64 C calls pass early integer args in `%rdi`, `%rsi`, `%rdx`, … and
  take integer returns from `%rax`.
- A hand-written asm function is just a global label that respects that
  contract.
- Prefer caller-saved scratch for tiny leaves; preserve callee-saved if you
  touch them.
- Validate against `gcc -S` output and a harness, not against folklore.

## Lookup (not the lesson)

- `man 1 as`, `man 1 gcc`, `info as`
- Discover registers from your own Track A `gcc -S` artifacts (ex. 04–05)

Now open `TASK.md` and do the practice.
