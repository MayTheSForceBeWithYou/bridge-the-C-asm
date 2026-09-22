# 42 — Growing the program break

Before anonymous `mmap` dominated allocator designs, Unix heaps grew by moving the
*program break* — the end of the data segment — with `brk` / `sbrk`. Modern glibc still
uses the break for small allocations and `mmap` for large ones. This lesson makes that
boundary visible: you move the break yourself, use the new bytes, and optionally watch
what `malloc` does.

## What this lesson asks of you

From ordinary C (printf is fine), query the current break, grow it by 4096, write and
read a pattern in the new region, and print `ok`. You should be able to explain what
the break *is* and why growing it is not the same as `mmap` of an anonymous page.

## The program break

The break is the first address *past* the writable data the loader gave you (end of
BSS/heap segment in the classic picture). `sbrk(0)` returns the current break without
moving it. `brk(new_addr)` asks the kernel to set the break to `new_addr`. `sbrk(delta)`
is a wrapper that adjusts by a signed delta.

When the break moves upward, the addresses in between become usable anonymous memory
backed by the kernel like the rest of your heap. When it moves downward, you shrink —
dangerous if anything still points into the discarded range.

This exercise uses libc wrappers (`sbrk`/`brk`) on purpose. Track D's freestanding
syscall work still applies: under the hood these are `__NR_brk` (12 on this machine).

## Contrast with malloc

A large `malloc` (megabytes) often becomes an `mmap` inside glibc; tiny allocations
often come from the break-managed arena. Optional `strace -e brk,mmap` after
`malloc(1<<20)` shows which path you got. Without `strace`, read `man 2 brk` and the
malloc notes in `info libc` — do not treat missing `strace` as a blocker.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` (libc/`printf` OK — this lab is about the break) |
| `./prog` | old/new addresses + `ok`, exit 0 |
| optional `strace -e brk,mmap ./prog` | big `malloc` → `brk` vs `mmap` |

## Reading printed break addresses

```text
old=0x55a1b2c3d000
new=0x55a1b2c3e000
ok
```

| Token | How to read it |
| ----- | -------------- |
| `old=` | break *before* grow |
| `new=` | break *after* grow |
| difference | `0x1000` if you grew 4096 |

**Navigation:** do not skim only for `ok` — confirm the hex gap.

**Rejected wrong reading:** `sbrk(0)` returns a pointer you can already write 4096 bytes
through. It is the boundary; space appears after a successful grow.

## Failure encoding (libc wrappers)

| Call | Failure |
| ---- | ------- |
| `sbrk` | `(void *)-1` |
| `brk` | `-1` |

**Rejected wrong reading:** `if (!p)` after `sbrk` — NULL is the wrong sentinel.

## `brk` versus anonymous `mmap` (exercise 41)

| | `brk`/`sbrk` | anonymous `mmap` |
| - | ------------ | ---------------- |
| What moves | end-of-heap break | separate VA range |
| Typical glibc use | small allocs | large allocs |
| Failure (libc) | `(void *)-1` / `-1` | `MAP_FAILED` |

## Optional strace after `malloc(1<<20)`

**Navigation:** `strace -e brk,mmap ./prog 2>&1` and search for `brk(` / `mmap(`.
Missing `strace` is not a blocker — pattern write/read remains required proof.

## Worked example

**The situation.** `prog.c` prints old and new break addresses, stores a pattern at the
old break, reloads it, prints `ok`, exits 0.

**Step 1 — query.** `void *old = sbrk(0);` gives the current break. The rejected wrong
reading is: "that pointer is already space I can write without growing." It is the
*boundary*; usable new space appears only after a successful grow.

**Step 2 — grow.** `brk((char *)old + 4096)` or `sbrk(4096)` should succeed. Check for
`(void *)-1`. Then write a pattern at `old` (or `old` after sbrk returns the *previous*
break — know which API you called). The rejected wrong reading is: "sbrk always returns
the new break." Historically `sbrk` returns the prior break; read the man page for the
wrapper you use and verify with a printed address.

**Step 3 — optional malloc peek.** Call `malloc(1<<20)` once and, if `strace` exists,
note `brk` vs `mmap`. Either outcome teaches the allocator's policy; your break demo
remains the required proof.

## Safe grow pattern (know which API you called)

```c
void *old = sbrk(0);
if (old == (void *)-1) /* fail */;
if (brk((char *)old + 4096) != 0) /* fail */;
void *neu = sbrk(0);
/* write pattern at old; expect neu == old + 4096 */
```

Historically `sbrk` returns the *previous* break on success. Verify with printed
addresses instead of assuming. **Rejected wrong reading:** "`sbrk` always returns the new
break."

## Why this lab allows libc

Unlike 40–41, `printf` is fine — you are studying the break, not freestanding purity. Do
not "prove nostdlib" here.

## Distinctions worth keeping straight

- **Break vs mmap region** — contiguous classic heap versus separately mapped VA ranges.
- **`sbrk(0)` query vs grow** — zero delta is inspection only.
- **Failure return** — `(void *)-1`, not NULL, for these interfaces.
- **Libc malloc policy** — size thresholds choose brk vs mmap; your tiny grow is not a
  full allocator.

## Deeper worked navigation (program break)

- `sbrk(0)` queries; grow creates usable bytes.
- Failure is `(void *)-1`, not NULL.
- Optional strace shows glibc choosing brk vs mmap for big malloc.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| printed old/new | difference 0x1000 for +4096 |
| pattern at old | survives after grow |
| strace -e brk,mmap | policy for large malloc |

### Ordered navigation moves

1. Print old.
2. Grow.
3. Print new; subtract.
4. Pattern write/read.
5. Optional malloc+strace.

### Rejected wrong readings (keep beside the artifact)

- `sbrk(0)` is already writable space.
- `if (!p)` detects sbrk failure.
- brk and mmap are the same syscall.

### Tool-line decoding reminders

- `%p` hex gap = grown bytes
- strace: name(args)=return
- sentinel `(void *)-1`

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (42)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - wrote through sbrk(0) before grow
- checked failure with !p
- mixed objects after malloc experiment

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

1. What does `sbrk(0)` return, and may you write through it before growing?
2. How do you detect a failed `brk`/`sbrk`?
3. Why might a 1 MiB `malloc` not show up as `brk` in a trace?
4. How does this differ from exercise 41's anonymous `mmap`?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- The program break is a movable end-of-heap boundary managed by `brk`/`sbrk`.
- Growing creates usable addresses; querying alone does not.
- Allocators mix break and `mmap`; observing both clarifies "where did this pointer come
  from?"
- Pattern write/read is how you prove the new region is real.

## Field-decoding recap for exercise 42

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 brk`, `man 2 sbrk`, `man 2 mmap`, `info libc`, `man 3 printf`

Now open `TASK.md` and do the practice.
