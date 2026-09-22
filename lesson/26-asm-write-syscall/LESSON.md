# 26 — `write` through a raw syscall

Printing without libc means asking the kernel to copy bytes to a file
descriptor. The `write` syscall uses the same register contract as `exit`, with
three arguments: descriptor, buffer pointer, and length. This lesson extends
your freestanding vocabulary from "terminate" to "emit bytes." By the end you
should map fd/buf/count onto `%rdi`/`%rsi`/`%rdx`, load `__NR_write` into
`%rax`, and recognize that `write` does not stop at NUL for you.

## What this lesson asks of you

Issue `__NR_write` (1 on Linux x86-64) yourself. The exercise ships two shapes —
pick one or do both:

- **Freestanding:** `_start` writes a message to fd 1, then `__NR_exit`.
- **C-callable:** `long my_write(int fd, const void *buf, unsigned long n)` that
  performs the syscall and returns the kernel's result in `%rax`.

Default `make` builds the C-callable harness; `make freestanding` builds the
`_start` path.

## Argument mapping for `write`

Decode the call as fields — same discipline as exercise 21:

| Meaning | Syscall register | C-callable ABI on entry to `my_write` |
| ------- | ---------------- | ------------------------------------- |
| syscall number | `%rax` ← 1 | you must load this; not set by C |
| fd | `%rdi` | already `%rdi` |
| buf | `%rsi` | already `%rsi` |
| count | `%rdx` | already `%rdx` |
| invoke | `syscall` | same |
| result | `%rax` | bytes written or negative errno-style |

Notice how conveniently the C ABI and the syscall ABI align for these first
three integer arguments. That alignment is why a thin `my_write` wrapper is
mostly "set `%rax` and `syscall`" — but remember syscall arg 4 would be `%r10`,
not `%rcx`, if you needed it later.

Confirm the number with `ausyscall write` or `/usr/include/asm/unistd_64.h`.

**Rejected wrong reading:** "I must also `call write` in libc." That defeats the
exercise; raw `syscall` is the point.

**Rejected wrong reading:** "After `syscall`, I should move something into
`%rax` to 'return success'." The kernel already left the byte count (or error)
in `%rax`. Overwriting it makes the harness think the write failed even when
bytes appeared on the terminal.

## Buffers and lengths

The kernel does not know about C string terminators for `write`. You must pass
an explicit length. In freestanding code that often means a label on a
`.asciz` / `.ascii` message plus a length you compute (current address minus
label) or hard-code. Writing past the intended bytes is your bug; the kernel
will happily send whatever range you name if the memory is readable.

| Fd | Usual meaning |
| -- | ------------- |
| 0 | standard input |
| 1 | standard output |
| 2 | standard error |

Using the wrong fd still "works" but confuses harness output.

### What `make` maps to

| You type | Builds | Check |
| -------- | ------ | ----- |
| `make` / `make run` | C-callable `my_write` + harness | stdout + exit 0 |
| `make freestanding` | `_start` in `write_n.s` | message then exit status |
| `objdump -d` | listing | search `<my_write>:` or `<_start>:` |

## How to read the sequence in `objdump -d`

**C-callable path — navigation:** search `<my_write>:`. Expect roughly:

```text
0000000000001179 <my_write>:
    1179:	48 c7 c0 01 00 00 00 	mov    $0x1,%rax
    1180:	0f 05                	syscall
    1182:	c3                   	ret
```

| Token | Meaning |
| ----- | ------- |
| `$0x1` in `%rax` | `__NR_write` |
| No stores to `%rdi`/`%rsi`/`%rdx` | args already correct from C |
| `syscall` | kernel entry |
| `ret` with `%rax` untouched | return byte count to harness |

**Freestanding path — navigation:** search `<_start>:`. Expect: loads into
`%rax`/`%rdi`/`%rsi`/`%rdx` for write, `syscall`, then reloads `%rax`/`%rdi`
for exit, `syscall` again. There should be no relied-upon `ret`.

**Rejected wrong reading:** "After `write` I can `ret` from `_start`." Same
lesson as exercise 21: no caller; exit yourself.

## Worked example

**The situation.** You open `my_write.s` (and optionally `write_n.s`) with stubs.

**Step 1 — C-callable path.** Arguments are already in `%rdi`, `%rsi`, `%rdx`.
Move `1` into `%rax`, execute `syscall`, `ret` with `%rax` unchanged from the
kernel. Mentally simulate: harness asks to write 5 bytes to fd 1; after
`syscall`, `%rax` should be 5 on success.

**Step 2 — check the harness.** Output should appear on stdout; exit status 0.
If bytes appear but the harness fails, you probably clobbered `%rax` after
`syscall`.

**Step 3 — freestanding optional path.** `_start` must set all three write args
explicitly (fd 1, address of message, length), syscall, then load `__NR_exit`
and a status into `%rax`/`%rdi` and syscall again. Decode each `mov` immediate:
is it a syscall number, an fd, or a length?

**Step 4 — compare to exercise 21.** Same register contract; different number in
`%rax` and more arguments. If you find yourself searching man pages for "how
write works" before drawing the register table, stop — draw the table first.

## Distinctions worth keeping straight

- **`write(2)` libc vs `__NR_write`** — same kernel operation; different entry
  (PLT/wrapper vs your `syscall`).
- **Length vs NUL-terminated string** — `write` wants a count.
- **Return value in `%rax`** — success is bytes written; do not overwrite before
  `ret` from `my_write`.
- **Fd 1 vs a path string** — this syscall writes to an open descriptor, not a
  filename (`open` is a different number).
- **C-callable `ret` vs freestanding `exit`** — only the latter ends the process
  without CRT.

## Check yourself

1. What value goes in `%rax` for `write`, and where do you confirm it?
2. Which three registers carry fd, buffer, and count?
3. Why is returning from `_start` after a successful write still wrong?
4. How does the C-callable wrapper's entry state differ from freestanding
   `_start`?
5. On a `mov $0x1,%rax` line before `syscall` for write, is `0x1` the fd or the
   syscall number?
6. What search string finds `my_write` in a listing?

## Key takeaways

- `__NR_write` is 1; args use `%rdi`, `%rsi`, `%rdx`; invoke with `syscall`.
- `write` needs an explicit byte count; it does not stop at NUL for you.
- C ABI and syscall ABI coincide for these three args — convenient, not
  universal (watch `%r10` later).
- Preserve the kernel's `%rax` when returning from a C-callable wrapper.
- Freestanding programs still end with `exit` after I/O.


## Length of a freestanding message

In `write_n.s` style code you often see:

```text
msg:
    .ascii "hello
"
msg_end:
```

Length can be the difference of labels (`msg_end - msg`) as an immediate, or a
hand-counted constant. The kernel will write exactly that many bytes. Including
the newline in the length is how the terminal gets a line break; forgetting it
still "works" but looks wrong next to harness expectations.

**Recognition in listings:** immediates loaded into `%rdx` before `syscall` are
counts; immediates loaded into `%rax` are syscall numbers; immediates loaded
into `%rdi` for write are fds (usually `1`). Say which is which on each `mov`
before you run.

## Lookup (not the lesson)

- `man 2 write`, `man 2 syscall`, `man 2 syscalls`
- `/usr/include/asm/unistd_64.h`, `ausyscall`
- `man 1 as`, `man 1 ld`, `man 1 objdump`

Now open `TASK.md` and do the practice.
