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

## Mapping `make` to commands and files

| You type | Rough equivalent | Open / check afterward |
| -------- | ---------------- | ---------------------- |
| `make` | `as` + freestanding link | the named binary (e.g. `./cat_task`) |
| run the binary | — | stdout matches `TASK.md` |
| `objdump -d ./…` | disassembly | search `<_start>:` |
| `nm ./…` | symbol table | no `printf` / `__libc_start_main` |
| `readelf -d ./…` | dynamic tags | no `NEEDED libc.so.6` (or no dynamic section) |
| optional `strace -e open,openat,read,write,close,exit ./…` | syscall trace | open → read/write loop → close → exit |

`make` here does **not** pull CRT the way `gcc hello.c` does.

## Decoding one `objdump -d` syscall setup

```text
  401014:	48 c7 c0 00 00 00 00 	mov    $0x0,%rax
  40101b:	0f 05                	syscall
```

| Column | Example | What it is |
| ------ | ------- | ---------- |
| Instruction address | `401014:` | Where this instruction lives |
| Raw bytes | `48 c7 c0 …` | Encoding |
| Mnemonic | `mov $0x0,%rax` | Decode — here `__NR_read` |

Addresses vary with PIE/layout; **column meaning** does not.

**Rejected wrong reading:** the left-column address is the syscall number.

## Decoding `readelf -d` when you expect no libc

| Observation | Meaning |
| ----------- | ------- |
| no dynamic section | static freestanding — success |
| `NEEDED … libc.so.6` | you linked libc — not freestanding |
| `ldd` says not a dynamic executable | success for static freestanding |

**Navigation:** search `readelf -d` output for the substring `NEEDED`.

## Register checklist for the five syscalls

| Syscall | `%rax` | `%rdi` | `%rsi` | `%rdx` |
| ------- | ------ | ------ | ------ | ------ |
| `open` | 2 | path | flags (`0`) | mode (ignored if not creating) |
| `read` | 0 | fd | buffer | count |
| `write` | 1 | fd (`1`) | buffer | count |
| `close` | 3 | fd | — | — |
| `exit` | 60 | status | — | — |

Arg4+ would use `%r10`, `%r8`, `%r9` — not needed here, but never invent arg4 in `%rcx`.

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

## Reading `%rax` after each call (signed, not "non-zero means good")

After `syscall`, interpret `%rax` as a signed 64-bit quantity for these I/O calls:

| Result | Meaning |
| ------ | ------- |
| `> 0` | success quantity (fd, byte count, …) |
| `0` from `read` | EOF — stop the copy loop |
| `< 0` | negated errno (e.g. `-2` = `-ENOENT`) |

Viewed as unsigned, a negative errno looks like a huge address. **Rejected wrong
reading:** treat that huge unsigned value as a buffer pointer or fd.

**Rejected wrong reading:** always `write` the full buffer capacity. Write *exactly* the
count `read` returned, or you replay stale bytes past live data.

## Path string placement

Put `TASK.md` plus a terminating NUL in `.rodata` (or an explicit labeled blob). Load the
*address* of that label into `%rdi` for `open` — often `lea path(%rip), %rdi` under PIE.
**Rejected wrong reading:** load the characters themselves as an immediate path.

## Distinctions worth keeping straight

- **Syscall ABI vs C ABI for calls** — `%r10` vs `%rcx` is the classic footgun.
- **Libc `open` vs `__NR_open`** — wrappers can add `openat`, cancellation, etc.; your
  freestanding path talks to the kernel number directly.
- **Zero from `read` vs error** — zero is EOF; negative (as unsigned, a huge value) is
  failure. Check the signed interpretation.
- **`exit` syscall vs returning from `main`** — with no libc, falling off `_start` is
  undefined; you must exit.

## Deeper worked navigation (freestanding cat)

- Treat the five syscalls as one story: open → read/write loop → close → exit.
- EOF is a successful `read` returning 0 — not an errno.
- Freestanding proof is about dependencies/symbols, not about `nm` being empty.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| objdump -d | `<_start>:` then mov-imm into `%rax` before `syscall` |
| readelf -d | no `NEEDED` libc, or no dynamic section |
| stdout | byte-identical to `TASK.md` |
| optional strace | open/read/write/close/exit sequence |

### Ordered navigation moves

1. Search `<_start>:`.
2. Find `mov $2,%rax` (or equivalent) for open.
3. Confirm write uses the read count, not buffer capacity.
4. Run `readelf -d` and search `NEEDED`.

### Rejected wrong readings (keep beside the artifact)

- Left-column objdump address is the syscall number.
- Always write the full buffer size.
- Path goes in `%rsi` because fopen docs show a mode there.

### Tool-line decoding reminders

- objdump: address | bytes | mnemonic
- readelf -d: look for tag name `NEEDED`
- signed `%rax`: 0 = EOF on read; negative = errno

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (40)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - wrong arg register for open path
- write full buffer capacity
- forgot exit and fell off _start

    For each tick: write the *recognition* fix (which register/column/anchor) in one line.
    That diary is how this track sticks.

    ## Makefile → command → file (recap)

    | You type | Produces / runs | Open next |
    | -------- | --------------- | --------- |
    | `make` / `make bin` | exercise binary | run it; note exit status |
    | `make clean` | removes objects | before changing `O=` / flags |
    | `make asm` / `make disasm` (if any) | listing view | search the label you care about |
    | tools in Lookup | field dumps | decode columns, do not skim blobs |

    Remember: a disasm target usually *views* bytes already linked — it is not a new
    mysterious compile stage (lesson 01's `.lst` rule).

## Check yourself

1. Which register holds the syscall number? Which holds the first argument?
2. Why does the syscall convention use `%r10` for the fourth argument?
3. What does `read` returning 0 mean in the copy loop?
4. How would you confirm with `nm`/`readelf` that the binary did not pull in libc?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

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
