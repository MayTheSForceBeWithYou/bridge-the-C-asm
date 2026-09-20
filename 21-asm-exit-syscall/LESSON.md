# 21 — `_start` and the exit syscall

When there is no C runtime, nothing calls `main` and nothing turns `return 0`
into a process exit. The kernel only understands syscalls: a number in `%rax`,
arguments in fixed registers, and the `syscall` instruction. This lesson teaches
that freestanding contract so you can end a process with a chosen status without
libc.

## What this lesson asks of you

Build a tiny ELF that starts at `_start`, loads `__NR_exit` and a status code,
invokes `syscall`, and never returns. The competence is naming *why* each
register matters and why `ret` from `_start` is nonsense — not memorizing every
syscall number.

`TASK.md` is the practice. Headers and man pages confirm spellings once you know
what you are looking for.

## Entry without CRT

The linker looks for a default entry symbol named `_start`. With `gcc` as the
driver, the C runtime (CRT) supplies `_start`, which eventually calls your
`main`. With a freestanding link (`ld` directly, or `gcc -nostdlib` and your own
entry), *you* provide `_start`.

There is no caller waiting for a return value from `_start`. Returning would pop
whatever sits above the initial stack and jump into garbage. The only clean end
is asking the kernel to terminate the process: Linux x86-64 `exit` (syscall
number 60 on this ABI).

## The Linux x86-64 syscall register contract

On this machine the kernel expects:

- Syscall number in `%rax`.
- Arguments in `%rdi`, `%rsi`, `%rdx`, `%r10`, `%r8`, `%r9` in that order (note
  `%r10`, not `%rcx` — `syscall` itself uses `%rcx` and `%r11`).
- The instruction `syscall` transfers control; the return value comes back in
  `%rax` (negative values encode errno-style failures for many calls).

For `exit`, only the first argument matters: the status code in `%rdi`. After a
successful `exit` there is no "after" — the process is gone.

Confirm the number in `/usr/include/asm/unistd_64.h` or with `ausyscall exit`
rather than trusting memory; the lesson is the *mechanism*, not the digit.

## What `ld` needs that `gcc` usually hides

A normal `gcc hello.c -o hello` run assembles, links CRT objects, and pulls in
libc. A freestanding exit binary typically needs you (or the Makefile) to:

- assemble your `.s` into a `.o`;
- link with an entry at `_start` and without the usual CRT;
- skip shared libc if you are not calling it.

`readelf -h` on the result should show an executable (or PIE DYN, depending on
flags) with an entry address. `objdump -d` should show `_start` containing a
load into `%rax`, a load into `%rdi`, and `syscall` — not a `call` into libc
`exit`.

## Worked example

**The situation.** You open a stub `exit_n.s` that declares `.global _start` and
a bare `_start:` label, then build with the exercise Makefile.

**Step 1 — picture the three moves.** Before `syscall`, `%rax` must hold 60 and
`%rdi` the status (for example 42). In AT&T syntax that is moves *into* those
registers, then `syscall`. The rejected wrong reading is: "I can `call exit`
like in C." Without libc linked, that symbol is not there; even with it, this
exercise is about the raw kernel path.

**Step 2 — run and check status.** `./exit_n; echo $?` (or `make run`) should
print the status you loaded into `%rdi`. The rejected wrong reading is: "exit
status is whatever `%rax` held." For this syscall the status is the *argument*
in `%rdi`; `%rax` was only the syscall selector.

**Step 3 — inspect the binary.** In `objdump -d`, confirm there is no `ret` at
the end of `_start` that you rely on. In `readelf -h`, note the entry point.
The rejected wrong reading is: "the entry must be named `main`." The kernel and
dynamic loader care about the ELF entry address; the *symbol* the linker uses
by default is `_start` unless you override it.

## Distinctions worth keeping straight

- **`_start` vs `main`** — entry the linker/CRT use versus the C-level function
  CRT calls after setup.
- **Syscall args vs C ABI args** — overlapping registers for the first few
  integers, but the fourth syscall arg is `%r10`, and you never "return" from
  `exit`.
- **`syscall` vs `int $0x80`** — this track is Linux x86-64 `syscall`; old 32-bit
  conventions are a different world.
- **Status in `%rdi` vs return in `%rax`** — do not swap them when reading
  disassembly.

## Check yourself

1. Why is a `ret` at the end of `_start` the wrong way to "finish"?
2. Which register holds `__NR_exit`, and which holds the process status?
3. Why might `gcc exit_n.s -o exit_n` without freestanding flags pull in CRT
   you did not write?
4. After `syscall` for `exit`, what code runs next in your process?

If any answer is fuzzy, re-read above before opening man pages for flag
spellings.

## Key takeaways

- Freestanding programs own `_start` and must end with a syscall, not `ret`.
- Linux x86-64 syscalls: number in `%rax`, args in `%rdi`…`%r9` (with `%r10`
  fourth), invoked by `syscall`.
- `__NR_exit` is 60 on this ABI; status is the first argument.
- `gcc` normally supplies CRT; freestanding builds make the entry and exit path
  your problem.

## Lookup (not the lesson)

- `man 2 syscall`, `man 2 syscalls`, `man 2 exit`
- `man 1 as`, `man 1 ld`, `man 5 elf`
- `/usr/include/asm/unistd_64.h`, `ausyscall`

Now open `TASK.md` and do the practice.
