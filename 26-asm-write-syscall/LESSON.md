# 26 — `write` through a raw syscall
<!-- concepts: c.p5.asm.syscall -->

Printing without libc means asking the kernel to copy bytes to a file
descriptor. The `write` syscall uses the same register contract as `exit`, with
three arguments: descriptor, buffer pointer, and length. This lesson extends
your freestanding vocabulary from "terminate" to "emit bytes."

## What this lesson asks of you

Issue `__NR_write` (1 on Linux x86-64) yourself. The exercise ships two shapes —
pick one or do both:

- **Freestanding:** `_start` writes a message to fd 1, then `__NR_exit`.
- **C-callable:** `long my_write(int fd, const void *buf, unsigned long n)` that
  performs the syscall and returns the kernel's result in `%rax`.

Default `make` builds the C-callable harness; `make freestanding` builds the
`_start` path.

## Argument mapping for `write`

| Meaning | Syscall register | C-callable ABI on entry |
| ------- | ---------------- | ----------------------- |
| fd | `%rdi` | already `%rdi` (`int`, zero-/sign-extended as usual) |
| buf | `%rsi` | already `%rsi` |
| count | `%rdx` | already `%rdx` |

Load `%rax` with 1, then `syscall`. On return, `%rax` is the byte count or a
negative errno-style value. The harness checks for a non-negative byte count.

Notice how conveniently the C ABI and the syscall ABI align for these first
three integer arguments. That alignment is why a thin `my_write` wrapper is
mostly "set `%rax` and `syscall`" — but remember syscall arg 4 would be `%r10`,
not `%rcx`, if you needed it later.

## Buffers and lengths

The kernel does not know about C string terminators for `write`. You must pass
an explicit length. In freestanding code that often means a label on a
`.asciz` / `.ascii` message plus a length you compute (current address minus
label) or hard-code. Writing past the intended bytes is your bug; the kernel
will happily send whatever range you name if the memory is readable.

Fd 1 is standard output for a normal process. Fd 2 is standard error. Using the
wrong fd still "works" but confuses harness output.

## Worked example

**The situation.** You open `my_write.s` (and optionally `write_n.s`) with stubs.

**Step 1 — C-callable path.** Arguments are already in `%rdi`, `%rsi`, `%rdx`.
Move `1` into `%rax`, execute `syscall`, `ret` with `%rax` unchanged from the
kernel. The rejected wrong reading is: "I must also `call write` in libc." That
would defeat the exercise; raw `syscall` is the point.

**Step 2 — check the harness.** Output should appear on stdout; exit status 0.
If you leave `%rax` as something else after tinkering, the harness may see a
failed write even when bytes appeared (or the reverse).

**Step 3 — freestanding optional path.** `_start` must set all three write args
explicitly, syscall, then load `__NR_exit` and a status into `%rax`/`%rdi` and
syscall again. The rejected wrong reading is: "after `write` I can `ret`." Same
lesson as exercise 21: no caller; exit yourself.

## Distinctions worth keeping straight

- **`write(2)` libc vs `__NR_write`** — same kernel operation; different entry
  (PLT/wrapper vs your `syscall`).
- **Length vs NUL-terminated string** — `write` wants a count.
- **Return value in `%rax`** — success is bytes written; do not overwrite before
  `ret` from `my_write`.
- **Fd 1 vs a path string** — this syscall writes to an open descriptor, not a
  filename (`open` is a different number).

## Check yourself

1. What value goes in `%rax` for `write`, and where do you confirm it?
2. Which three registers carry fd, buffer, and count?
3. Why is returning from `_start` after a successful write still wrong?
4. How does the C-callable wrapper's entry state differ from freestanding
   `_start`?

## Key takeaways

- `__NR_write` is 1; args use `%rdi`, `%rsi`, `%rdx`; invoke with `syscall`.
- `write` needs an explicit byte count; it does not stop at NUL for you.
- C ABI and syscall ABI coincide for these three args — convenient, not
  universal (watch `%r10` later).
- Freestanding programs still end with `exit` after I/O.

## Lookup (not the lesson)

- `man 2 write`, `man 2 syscall`, `man 2 syscalls`
- `/usr/include/asm/unistd_64.h`, `ausyscall`
- `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
