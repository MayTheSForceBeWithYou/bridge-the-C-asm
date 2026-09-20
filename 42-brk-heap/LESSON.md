# 42 — Growing the program break
<!-- concepts: c.p2.heap.malloc, c.p5.asm.brk -->

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

## Distinctions worth keeping straight

- **Break vs mmap region** — contiguous classic heap versus separately mapped VA ranges.
- **`sbrk(0)` query vs grow** — zero delta is inspection only.
- **Failure return** — `(void *)-1`, not NULL, for these interfaces.
- **Libc malloc policy** — size thresholds choose brk vs mmap; your tiny grow is not a
  full allocator.

## Check yourself

1. What does `sbrk(0)` return, and may you write through it before growing?
2. How do you detect a failed `brk`/`sbrk`?
3. Why might a 1 MiB `malloc` not show up as `brk` in a trace?
4. How does this differ from exercise 41's anonymous `mmap`?

## Key takeaways

- The program break is a movable end-of-heap boundary managed by `brk`/`sbrk`.
- Growing creates usable addresses; querying alone does not.
- Allocators mix break and `mmap`; observing both clarifies "where did this pointer come
  from?"
- Pattern write/read is how you prove the new region is real.

## Lookup (not the lesson)

- `man 2 brk`, `man 2 sbrk`, `man 2 mmap`, `info libc`, `man 3 printf`

Now open `TASK.md` and do the practice.
