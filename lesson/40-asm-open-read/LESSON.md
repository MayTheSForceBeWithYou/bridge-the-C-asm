# 40 — Raw open/read/write/close

Libc is a convenience layer. Under it, Linux still speaks one language to user code:
the `syscall` instruction with a number in `%rax` and arguments in the System V
register slots. This lesson teaches you to *cat* a file with only those primitives so
you can see what `cat` and `printf` are really asking the kernel to do.

## What this lesson asks of you

Build a freestanding `_start` that opens this exercise's `TASK.md`, copies its bytes
to stdout, closes the fd, and exits — no libc, no `main`. Competence is naming which
register carries which argument for each call, and knowing what a zero-length `read`
means (EOF), not memorizing every flag bit.

`TASK.md` is the practice. Man pages name fields and errno values; they do not replace
the register story below.

## The x86-64 Linux syscall contract

On this ABI, a syscall is:

1. Put the syscall number in `%rax`.
2. Put arguments in `%rdi`, `%rsi`, `%rdx`, `%r10`, `%r8`, `%r9` (note `%r10`, not
   `%rcx` — the `syscall` instruction clobbers `%rcx`/`%r11`).
3. Execute `syscall`.
4. Read the result from `%rax` (non-negative success, or a negated errno).

Numbers for this exercise (from `/usr/include/asm/unistd_64.h` on this machine):
`read` is 0, `write` is 1, `open` is 2, `close` is 3, `exit` is 60. You look them up;
you do not invent them.

`open` needs a path pointer, flags (0 for read-only is enough here), and often a mode
that is ignored when not creating. `read`/`write` take `(fd, buf, count)`. `close`
takes the fd. `exit` takes the status. Same register order every time.

## Freestanding entry: `_start`, not `main`

Without libc there is no `__libc_start_main` to call your `main`. The ELF entry symbol
is `_start`. The kernel has already mapped your segments and pointed `%rsp` at the
initial stack; you own the rest. Assemble with `as`, link with `ld` (or
`gcc -nostdlib`), and `nm` should show your syscalls — not `printf`.

A small `.bss` or stack buffer is fine for the read loop. Relative path `TASK.md` is
resolved from the process cwd when you run `./cat_task`.

## The copy loop

Open once. Then: `read` into the buffer; if the return value is zero, you hit EOF; if
negative, treat as error and exit non-zero; otherwise `write` that many bytes to fd 1
and repeat. Close the input fd. Exit 0.

Partial writes are rare on a pipe/tty for small buffers but real on some fds — for this
lab, writing what `read` returned is the honest loop.

## Worked example

**The situation.** You draft `cat_task.s`, link freestanding, run `./cat_task`, and
compare stdout to `TASK.md`.

**Step 1 — open.** Load `__NR_open` (2) into `%rax`, a pointer to the string
`TASK.md` into `%rdi`, flags 0 into `%rsi`, and `syscall`. A non-negative `%rax` is
the fd. The rejected wrong reading is: "I can pass the path in `%rsi` because that is
where libc's `fopen` documentation shows the mode." Argument order for the *syscall*
is what matters, not the libc wrapper's C prototype spelling.

**Step 2 — one successful read/write.** After `read` returns e.g. 200, your `write`
must use count 200 and fd 1. The rejected wrong reading is: "always write the full
buffer size." That reprints stale bytes past EOF.

**Step 3 — EOF and exit.** A `read` that returns 0 ends the loop; then `close` and
`exit` with 60 in `%rax`. Optional: if `strace` is installed, `-e open,openat,read,write,close`
should show that shape; if not, reason from the code — `strace` is optional here.

## Distinctions worth keeping straight

- **Syscall ABI vs C ABI for calls** — `%r10` vs `%rcx` is the classic footgun.
- **Libc `open` vs `__NR_open`** — wrappers can add `openat`, cancellation, etc.; your
  freestanding path talks to the kernel number directly.
- **Zero from `read` vs error** — zero is EOF; negative (as unsigned, a huge value) is
  failure. Check the signed interpretation.
- **`exit` syscall vs returning from `main`** — with no libc, falling off `_start` is
  undefined; you must exit.

## Check yourself

1. Which register holds the syscall number? Which holds the first argument?
2. Why does the syscall convention use `%r10` for the fourth argument?
3. What does `read` returning 0 mean in the copy loop?
4. How would you confirm with `nm`/`readelf` that the binary did not pull in libc?

## Key takeaways

- Raw I/O is a short sequence of numbered syscalls with a fixed register contract.
- Freestanding programs enter at `_start` and must exit explicitly.
- EOF is a zero-length successful `read`, not an error code.
- Optional tools like `strace` confirm the story; the lesson is the register-level
  mechanism.

## Lookup (not the lesson)

- `man 2 syscall`, `man 2 syscalls`, `man 2 open`, `man 2 read`, `man 2 write`,
  `man 2 close`, `man 2 exit`
- `/usr/include/asm/unistd_64.h`, `ausyscall`
- `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
