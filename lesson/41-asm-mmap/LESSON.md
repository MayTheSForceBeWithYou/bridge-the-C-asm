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

## Mapping `make` to commands and files

| You type | Open / check afterward |
| -------- | ---------------------- |
| `make` | freestanding `./mmap_demo` (name per Makefile) |
| `./mmap_demo; echo $?` | exit status 0 |
| `objdump -d ./…` | search `<_start>:`; find six-arg setup |
| optional `strace -e mmap,munmap ./…` | one map, one unmap |

## Six-argument register map for this `mmap`

| Arg | Register | Value here |
| --- | -------- | ---------- |
| number | `%rax` | 9 |
| addr | `%rdi` | 0 |
| length | `%rsi` | 4096 |
| prot | `%rdx` | 3 |
| flags | **`%r10`** | `0x22` |
| fd | `%r8` | `-1` |
| offset | `%r9` | 0 |

**Rejected wrong reading:** fourth argument goes in `%rcx` like a normal `call`. For
`syscall`, arg4 is `%r10`.

## Success versus failure in `%rax`

Success: page-aligned user pointer. Failure: small negative errno magnitude. Libc turns
that into `MAP_FAILED`; raw `syscall` does not.

Treat `%rax` as signed: about `-1`…`-4095` means errno — do not store through it.

**Rejected wrong reading:** any non-zero return is a valid pointer.

## Decoding optional `strace` mmap lines

```text
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f...
```

| Field | Meaning |
| ----- | ------- |
| `NULL` | kernel chooses VA |
| `4096` | length |
| prot/flags | permissions and anonymous private |
| `-1`, `0` | fd/offset unused for anonymous |
| after `=` | mapped VA on success |

**Navigation:** `strace -e mmap,munmap ./mmap_demo 2>&1` then search for `mmap(`.

## Pattern round-trip as proof

Store `0xDEADBEEF` (or similar) at offset 0, reload, compare. Only then `munmap` and
`exit 0`. That proves writable memory — not an unchecked error code.

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

## `munmap` contract (decode both arguments)

| Arg | Register | Value |
| --- | -------- | ----- |
| number | `%rax` | 11 |
| addr | `%rdi` | pointer `mmap` returned |
| length | `%rsi` | same length you mapped (4096) |

**Rejected wrong reading:** "pass only the address; the kernel remembers the size." You
must pass the length.

## Objectdump navigation for six-arg setup

Search `<_start>:`. Expect a cluster of `mov` immediates into `%rax`, `%rdi`, `%rsi`,
`%rdx`, `%r10`, `%r8`, `%r9` before `syscall`. Confirm the `%r10` immediate is `0x22`.

**Rejected wrong reading:** left-column addresses are the mapped page VA. Those columns
are instruction addresses; the mapped VA appears in `%rax` only after `mmap` returns at
run time.

## Distinctions worth keeping straight

- **Anonymous vs file-backed** — anonymous is "give me RAM"; file-backed maps file
  bytes into the VA space.
- **`MAP_PRIVATE` vs shared** — private copy-on-write semantics; you are not sharing
  with other processes here.
- **Syscall failure encoding** — negative errno in `%rax` vs libc's `MAP_FAILED`.
- **Page size** — 4096 is the common size used in this track; do not assume every
  architecture matches without checking.

## Deeper worked navigation (anonymous mmap)

- Six-arg `mmap` is the classic `%r10` footgun.
- Validate `%rax` before storing; pattern round-trip proves writability.
- `munmap` needs address *and* length.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| objdump -d | mov into `%r10` with `0x22` |
| runtime `%rax` | page-aligned pointer vs small negative |
| strace (optional) | mmap then munmap |

### Ordered navigation moves

1. Search `<_start>:`.
2. Confirm flags in `%r10`, not `%rcx`.
3. Store/load pattern.
4. munmap same length.

### Rejected wrong readings (keep beside the artifact)

- Fourth arg in `%rcx`.
- Non-zero `%rax` always means valid pointer.
- munmap needs only the address.

### Tool-line decoding reminders

- syscall arg4 = `%r10`
- signed failure window ≈ -1..-4095
- strace fields: addr,len,prot,flags,fd,offset,return

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (41)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - flags in %rcx not %r10
- stored through negative errno
- munmap without length

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

1. What goes in `%r10` for this `mmap` call?
2. Why is the file descriptor `-1` for an anonymous mapping?
3. How do you distinguish a successful mapped address from a failed syscall return?
4. What must you pass to `munmap` besides the address?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Anonymous `mmap` is the kernel primitive behind many allocators.
- Six-argument syscalls need `%r10`/`%r8`/`%r9` filled correctly.
- Always validate `%rax` before storing through a mapped pointer.
- `munmap` releases the VA range; exiting without it leaks until process death — still
  release it on the happy path for clarity.

## Field-decoding recap for exercise 41

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 mmap`, `man 2 munmap`, `man 2 syscall`
- `unistd_64.h`, `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
