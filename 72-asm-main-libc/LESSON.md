# 72 — `main` in asm, linked with libc
<!-- concepts: c.p5.asm.main, c.p5.abi.call -->

Freestanding `_start` programs talk to the kernel. Most real programs still
want libc: `puts`, `printf`, heap, and a civilized exit. This lesson has you
write all of `main` by hand while CRT and `-lc` stay in the link — so you meet
their contract from the callee side of `main` and the caller side of libc.

## What this lesson asks of you

Provide a global `main` in `hello_asm.s` with no C translation unit of your
own. Call `puts` and `printf`, then return 0 so CRT exits cleanly. Deliberately
break stack alignment once and watch where it dies. Compare your prologue to
`gcc -S` from exercise 01.

## Who calls `main`?

The ELF entry is still CRT `_start` (names vary by libc). That path sets up
TLS, libc internals, and eventually calls `main`. Your return value in `%eax` /
`%rax` becomes the process exit status after CRT runs `exit`-family cleanup.

So you are not freestanding: you must return (or `call exit`), keep ABI
alignment for libc calls, and use the same argument registers as any C function.
gdb `break main` / `bt` shows CRT frames above you — useful proof that you did
not become `_start`.

## Alignment at the first instruction of `main`

On entry, `%rsp ≡ 8 (mod 16)` because `call main` already pushed a return
address. Before you `call puts` or `call printf`, you need `%rsp ≡ 0 (mod 16)`.
A typical fix is `push %rbp` (and usually `mov %rsp, %rbp`), optionally with a
further `sub` of a multiple of 16 for locals.

If you strip the prologue pushes/subs and call libc at `-O0`, many builds die
inside the callee with a general-protection or segmentation fault when an SSE
spill uses a misaligned stack slot. Note *which* libc routine appears in the
backtrace — that is evidence the bug was alignment, not a bad format string.

## Talking to `puts` and `printf`

- `puts(greeting)`: pointer in `%rdi`, then `call puts`.
- `printf(fmt, greeting, 14)`: format in `%rdi`, then `%rsi`, then `%rdx` for
  the integer. Variadic functions also look at `%al` for the *number of vector
  (XMM) arguments*. For a call with no floating-point args, zero `%al` (often
  `xor %eax, %eax` before `call printf` when you do not otherwise need `%rax`).
  Leaving `%al` garbage can make `printf` touch random XMM state and crash or
  corrupt.

String literals belong in `.rodata` (exercise 74 drills directives). For this
lesson, follow the stub's labels for `greeting` and `fmt`.

## Worked example

**The situation.** Stub `hello_asm.s` links and may exit 0 silently until you
implement the calls.

**Step 1 — frame + `puts`.** Build a minimal aligned prologue, load the
greeting address into `%rdi`, `call puts`. Expected line: `hello from asm`. The
rejected wrong reading is: "I can pass the string in `%rax` because that is the
return register." Callees read `%rdi` for the first pointer arg.

**Step 2 — `printf` with `%al` cleared.** Set up three integer/pointer args and
zero `%al`, then `call printf`. Expected second line mentions `14` chars. The
rejected wrong reading is: "variadic is just like three normal args; `%al` does
not matter." On this ABI it does.

**Step 3 — return 0 and compare.** `xor %eax, %eax` (or `mov $0, %eax`),
epilogue, `ret`. Run `make disasm` and place your prologue next to exercise 01's
C `main` — same contract, different author.

## Distinctions worth keeping straight

- **`main` vs `_start`** — CRT entry versus the C-level function you write here.
- **Return from `main` vs `syscall` exit** — CRT turns your return into process
  status; freestanding code cannot lean on that.
- **`%al` for variadic vector count vs integer arg count** — integers live in
  GPRs; `%al` is not "number of total args."
- **Alignment symptom vs wrong symbol** — crashing inside `printf` often means
  your stack, not a missing PLT entry (a missing symbol would fail at link).

## Check yourself

1. What is `%rsp` mod 16 at the first instruction of `main`, and why?
2. Which register tells `printf` how many XMM args were passed?
3. Who calls `main`, and where does your return value go next?
4. Why might removing `push %rbp` alone cause a fault inside libc?

## Key takeaways

- Asm `main` still obeys the C ABI; CRT calls you and consumes your `%eax`.
- Align `%rsp` before every libc `call`; entry skew is 8 from the return
  address.
- Zero `%al` before variadic calls with no SSE args.
- Broken alignment often faults in the callee — use that when debugging.

## Lookup (not the lesson)

- `man 3 puts`, `man 3 printf`, `man 1 as`, `man 1 gcc`, `info as`
- Argument registers from your ex. 04–05 `.s` output

Now open `TASK.md` and do the practice.
