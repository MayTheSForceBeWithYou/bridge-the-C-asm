# 41 — Anonymous mmap from assembly

Heap-like memory does not require `malloc`. The kernel can map a fresh page into your
address space with `mmap`, and take it back with `munmap`. This lesson shows the raw
syscall path for an anonymous private mapping so you see what "a page of RAM" means at
the ABI boundary.

## What this lesson asks of you

From freestanding `_start`, map 4096 bytes readable and writable, store a recognizable
pattern, read it back, unmap, and exit 0 on success. You should be able to name the
`mmap` arguments in register order and explain why anonymous mappings use fd `-1`.

## What mmap is doing

`mmap` asks the kernel to establish a virtual memory region. For this exercise you want
anonymous memory (not backed by a file) that is private to your process:

- length 4096 (one page on typical x86-64 Linux configurations used here)
- protection `PROT_READ|PROT_WRITE` (value 3)
- flags `MAP_PRIVATE|MAP_ANONYMOUS` (value `0x22`)
- fd `-1`, offset 0
- address hint `NULL` (let the kernel choose)

Syscall number for `mmap` is 9; `munmap` is 11; `exit` is 60. Six arguments means you
will use `%r10`, `%r8`, and `%r9` as well as the usual first three registers.

On success `%rax` holds the mapped address (page-aligned). On failure it holds a small
negative errno magnitude — treat that as an error and exit non-zero rather than
storing through it.

## Pattern round-trip

Store something like `0xDEADBEEF` at offset 0 of the mapping (a 32-bit store is enough).
Load it back into a register and compare. Only if it matches do you `munmap` and exit 0.
That proves the mapping is real writable memory, not a successful-looking error code you
forgot to check.

## Worked example

**The situation.** Your `mmap_demo.s` is linked freestanding. You run it and expect exit
status 0.

**Step 1 — set up registers for mmap.** `%rax=9`, `%rdi=0`, `%rsi=4096`, `%rdx=3`,
`%r10=0x22`, `%r8=-1`, `%r9=0`, then `syscall`. The rejected wrong reading is: "fourth
argument goes in `%rcx` like a normal `call`." For `syscall`, the fourth argument is
`%r10`.

**Step 2 — check the return.** If `%rax` looks like `-E*` (high bits set if you view it
unsigned as a huge address), do not write to it. The rejected wrong reading is: "any
non-zero return is a valid pointer." Failed `mmap` returns a small negative value, not
NULL like some libc wrappers normalize to.

**Step 3 — store, reload, munmap.** After a matching reload, call `munmap` with the
address and length 4096. Optional strace (`-e mmap,munmap`) confirms the pair; without
strace, the exit code and your own checks are enough.

## Distinctions worth keeping straight

- **Anonymous vs file-backed** — anonymous is "give me RAM"; file-backed maps file
  bytes into the VA space.
- **`MAP_PRIVATE` vs shared** — private copy-on-write semantics; you are not sharing
  with other processes here.
- **Syscall failure encoding** — negative errno in `%rax` vs libc's `MAP_FAILED`.
- **Page size** — 4096 is the common size used in this track; do not assume every
  architecture matches without checking.

## Check yourself

1. What goes in `%r10` for this `mmap` call?
2. Why is the file descriptor `-1` for an anonymous mapping?
3. How do you distinguish a successful mapped address from a failed syscall return?
4. What must you pass to `munmap` besides the address?

## Key takeaways

- Anonymous `mmap` is the kernel primitive behind many allocators.
- Six-argument syscalls need `%r10`/`%r8`/`%r9` filled correctly.
- Always validate `%rax` before storing through a mapped pointer.
- `munmap` releases the VA range; exiting without it leaks until process death — still
  release it on the happy path for clarity.

## Lookup (not the lesson)

- `man 2 mmap`, `man 2 munmap`, `man 2 syscall`
- `unistd_64.h`, `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
