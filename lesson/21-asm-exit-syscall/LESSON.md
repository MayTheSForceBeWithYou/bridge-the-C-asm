# 21 — `_start` and the exit syscall

When there is no C runtime, nothing calls `main` and nothing turns `return 0`
into a process exit. The kernel only understands syscalls: a number in `%rax`,
arguments in fixed registers, and the `syscall` instruction. By the end of this
lesson you should be able to name why each register matters for `__NR_exit`,
find that sequence in `objdump -d`, and explain why `ret` from `_start` is
nonsense.

## What this lesson asks of you

Build a tiny ELF that starts at `_start`, loads `__NR_exit` and a status code,
invokes `syscall`, and never returns. The competence is naming *why* each
register matters and recognizing the three-instruction shape in a listing — not
memorizing every syscall number.

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

### What `make` is naming for you

This exercise's Makefile pulls shared rules from the track's common makefiles.
The targets are vocabulary, not magic:

| You type | Rough equivalent | File / check afterward |
| -------- | ---------------- | ---------------------- |
| `make` / `make bin` | assemble `.s` → `.o`, link freestanding | `./exit_n` |
| `make run` | run binary; print or echo status | shell shows `exit=…` or `$?` |
| `make disasm` | `objdump -d exit_n > exit_n.lst` (if present) | listing of `_start` |
| `readelf -h exit_n` | ELF header dump | Type / Entry point address |

`make` here does **not** pull in the usual CRT objects the way a normal
`gcc hello.c` does. That is the point of this freestanding build.

## The Linux x86-64 syscall register contract

On this machine the kernel expects a fixed layout. Decode it as fields, not as
"magic registers":

| Field | Register | For `exit` |
| ----- | -------- | ---------- |
| Syscall number | `%rax` | `__NR_exit` = 60 |
| Arg 1 | `%rdi` | process status (e.g. 42) |
| Arg 2 | `%rsi` | unused for `exit` |
| Arg 3 | `%rdx` | unused |
| Arg 4 | `%r10` | unused (note: **not** `%rcx`) |
| Arg 5–6 | `%r8`, `%r9` | unused |
| Invoke | `syscall` instruction | transfers to kernel |
| Return | `%rax` | never observed after successful `exit` |

Why `%r10` instead of `%rcx` for the fourth argument: the `syscall` instruction
itself clobbers `%rcx` (saved RIP) and `%r11` (saved RFLAGS). The ABI therefore
routes syscall arg 4 through `%r10`. For `exit` you only need `%rax` and `%rdi`,
but the rule matters as soon as you add `write`, `mmap`, or friends.

Confirm the number in `/usr/include/asm/unistd_64.h` or with `ausyscall exit`
rather than trusting memory; the lesson is the *mechanism*, not the digit.

**Rejected wrong reading:** "Exit status is whatever `%rax` held when I invoked
`syscall`." For this syscall the status is the *argument* in `%rdi`. `%rax` was
only the syscall selector (60). Mixing them up produces a process that exits
with status 60 (or whatever you left in `%rdi`) while you stare at the wrong
register in the listing.

## What a real `_start` looks like in `objdump -d`

After you build, open the listing (live `objdump -d exit_n` or a saved `.lst`).
**Navigation:** search for the label `<_start>:` (angle brackets and a colon).
Ignore any PLT or dynamic stubs if the link produced them — your freestanding
exit path should be a handful of instructions under `_start`.

A typical shape (addresses vary with PIE/layout; **columns** do not):

```text
0000000000401000 <_start>:
  401000:	48 c7 c0 3c 00 00 00 	mov    $0x3c,%rax
  401007:	48 c7 c7 2a 00 00 00 	mov    $0x2a,%rdi
  40100e:	0f 05                	syscall
```

Read one line left to right as four kinds of information:

| Column | Example | What it is |
| ------ | ------- | ---------- |
| Instruction address | `401000:` | Where *this* instruction lives in the image |
| Raw machine bytes | `48 c7 c0 3c 00 00 00` | Bytes stored there |
| Mnemonic + operands | `mov    $0x3c,%rax` | Human decoding |
| (optional hint) | — | `objdump` may annotate symbols; immediates here are bare |

On that first `mov`, `$0x3c` is 60 decimal — `__NR_exit`. On the second, `$0x2a`
is 42 decimal — the status you asked for. The `syscall` line has no "return"
path in the listing; control does not fall through to a `ret`.

**Rejected wrong reading:** "The left-column address is the exit status." The
left column is where the instruction lives. The status is the immediate loaded
into `%rdi` (or whatever value was in `%rdi` at `syscall`).

**Rejected wrong reading:** "I can `call exit` like in C." Without libc linked,
that symbol is not there. Even with libc, this exercise is about the raw kernel
path: number in `%rax`, status in `%rdi`, `syscall`.

## What `ld` needs that `gcc` usually hides

A normal `gcc hello.c -o hello` run assembles, links CRT objects, and pulls in
libc. A freestanding exit binary typically needs you (or the Makefile) to:

- assemble your `.s` into a `.o`;
- link with an entry at `_start` and without the usual CRT;
- skip shared libc if you are not calling it.

### How to read `readelf -h` for this binary

Run `readelf -h exit_n` and anchor on these fields (names from the tool's
headers):

| Field to find | What you want to recognize |
| ------------- | -------------------------- |
| `Type:` | `EXEC` (old-style executable) or `DYN` (PIE) — both can be runnable |
| `Entry point address:` | Should match the address of `_start` in `objdump -d` |
| `Machine:` | `Advanced Micro Devices X86-64` |

**Navigation:** in the `readelf -h` dump, search for the literal string
`Entry point`. Then in `objdump -d`, confirm that same hex appears as the
address of `<_start>:`.

**Rejected wrong reading:** "The entry must be named `main`." The kernel and
loader care about the ELF *entry address*. The *symbol* the linker uses by
default is `_start` unless you pass `-e` to override it. `main` is a C-level
convention CRT knows about — freestanding code never reaches it.

## Worked example

**The situation.** You open a stub `exit_n.s` that declares `.global _start` and
a bare `_start:` label, then build with the exercise Makefile.

**Step 1 — picture the three moves.** Before `syscall`, `%rax` must hold 60 and
`%rdi` the status (for example 42). In AT&T syntax that is moves *into* those
registers (source first: `mov $60, %rax`), then `syscall`. Spell immediates with
`$` and registers with `%`. The rejected wrong reading is treating the stub's
empty label as "almost done" — without those loads, `syscall` invokes whatever
garbage sits in `%rax` (often a crash or a random syscall).

**Step 2 — run and check status.** `./exit_n; echo $?` (or `make run`) should
print the status you loaded into `%rdi`. Shell `$?` is the process exit status
the kernel recorded — the same number that was in `%rdi` at `syscall` for
`exit` (low 8 bits for the shell). The rejected wrong reading is expecting
`$?` to equal 60 because that was in `%rax`.

**Step 3 — inspect the binary.** In `objdump -d`, search `<_start>:`. Confirm
there is no `ret` at the end that you rely on. Match the immediates to 60 and
your status. In `readelf -h`, note the entry point and match it to `_start`'s
address. The rejected wrong reading is: "linking freestanding just renames my
`.o`." Size, entry, and the absence of CRT symbols all change for a reason.

**Step 4 — change only the status immediate.** Rebuild and re-check `$?`. The
`mov` into `%rax` should stay 60; only `%rdi`'s immediate should track your new
status. If both change, you edited the wrong line.

## Distinctions worth keeping straight

- **`_start` vs `main`** — entry the linker/CRT use versus the C-level function
  CRT calls after setup.
- **Syscall args vs C ABI args** — overlapping registers for the first few
  integers, but the fourth syscall arg is `%r10`, and you never "return" from
  `exit`.
- **`syscall` vs `int $0x80`** — this track is Linux x86-64 `syscall`; old 32-bit
  conventions are a different world.
- **Status in `%rdi` vs selector in `%rax`** — do not swap them when reading
  disassembly or when checking `$?`.
- **Instruction address vs immediate operand** — left column vs `$0x…` in the
  mnemonic; only the latter (in `%rdi`) is the exit status.
- **Freestanding link vs `gcc` default** — default `gcc` invents CRT and libc;
  this exercise deliberately does not.

## Check yourself

Close this file and answer from memory, then verify against your artifacts:

1. Why is a `ret` at the end of `_start` the wrong way to "finish"?
2. Which register holds `__NR_exit`, and which holds the process status?
3. On a `mov $0x3c,%rax` line in `objdump -d`, which token is the instruction
   address and which is the syscall number?
4. What exact search string takes you to your entry in an `objdump -d` listing?
5. Why might `gcc exit_n.s -o exit_n` without freestanding flags pull in CRT
   you did not write?
6. After `syscall` for `exit`, what code runs next in your process?

If any answer is fuzzy, re-read the matching section above — do not open man
pages yet. When you need the exact flag for "no standard library," *then* look
up `-nostdlib`.

## Key takeaways

- Freestanding programs own `_start` and must end with a syscall, not `ret`.
- Linux x86-64 syscalls: number in `%rax`, args in `%rdi`…`%r9` (with `%r10`
  fourth), invoked by `syscall`.
- `__NR_exit` is 60 on this ABI; status is the first argument (`%rdi`).
- Hex on an `objdump` line is not all the same kind of number — learn the columns.
- `gcc` normally supplies CRT; freestanding builds make the entry and exit path
  your problem.

## Lookup (not the lesson)

Use these when you already know the question:

- `man 2 syscall`, `man 2 syscalls`, `man 2 exit`
- `man 1 as`, `man 1 ld`, `man 5 elf`
- `/usr/include/asm/unistd_64.h`, `ausyscall`
- `man 1 objdump`, `man 1 readelf` — listing and header field spellings

Now open `TASK.md` and do the practice.
