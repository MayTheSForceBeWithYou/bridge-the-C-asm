# 44 — Bump allocation without malloc

A bump (arena) allocator is the simplest real allocator: map one big region, hand out
consecutive slices by advancing a pointer, and free everything at once by discarding the
region. Linked-list nodes are an ideal client — many small same-lifetime objects, no
per-node `free`. This lesson rebuilds a tiny list on an `mmap` arena.

## What this lesson asks of you

`mmap` a chunk (e.g. 64 KiB), implement `arena_alloc` that bumps (with alignment you
choose), build a list of nodes with values 1, 2, 3 whose sum is 6, print `ok`, and
`munmap`. Your code must not call `malloc`/`free` for those nodes.

## Bump mechanics

Keep a base pointer, a `cursor` (or `used` byte count), and a capacity. Allocation of
`n` bytes (optionally rounded up to 8-byte alignment) returns `base + cursor` and adds
`n` to `cursor`, or fails if capacity would exceed. There is no per-object free — only
reset or unmap.

Nodes look like exercise 17: a value field and a `next` pointer. Link three nodes, walk
to sum, compare to 6. Libc may still be linked for `printf`; that is fine. Check *your*
source and `nm` for absence of `malloc`/`free` in *your* allocation path.

## Worked example

**The situation.** `./prog` prints `ok`. You `grep` your `.c` for malloc and find none
for the list.

**Step 1 — map the arena.** Anonymous RW `mmap` as in exercise 41 (libc `mmap` is fine
here). Save the pointer and length. The rejected wrong reading is: "I can bump-allocate
out of a stack buffer of 64 KiB safely for any size." Stack space is limited and ABI
fragile; this exercise wants an explicit mapped arena.

**Step 2 — alloc nodes.** `arena_alloc(sizeof(Node))` three times; fill `val` and
`next`. The rejected wrong reading is: "alignment does not matter on x86-64." Misaligned
pointers bite under `-O` sanitizers and some loads; align to at least `_Alignof(Node)`.

**Step 3 — sum and release.** Walk the list, check sum 6, `munmap` the arena. Do not
call `free` on node pointers — they were never `malloc`ed.

## Distinctions worth keeping straight

- **Bump free vs malloc free** — arena frees all or nothing; malloc frees per object.
- **Arena pointer vs heap pointer** — same VA usability; different lifetime protocol.
- **printf's malloc vs yours** — libc internals may allocate; your nodes must not.
- **Alignment** — bump without round-up is a classic bug.

## Check yourself

1. What state does a bump allocator keep, and what does "free" mean for it?
2. Why unmap the arena instead of freeing each node?
3. How do you verify you did not call `malloc` for nodes?
4. What goes wrong if `arena_alloc` forgets alignment?

## Key takeaways

- Bump arenas trade flexible free for speed and simplicity.
- `mmap` (+ `munmap`) is enough backing store for a lab arena.
- Lifetime grouping (all nodes die together) matches arena semantics.
- Alignment and capacity checks are part of a correct bump API.

## Lookup (not the lesson)

- `man 2 mmap`, `man 2 munmap`, `man 3 printf`

Now open `TASK.md` and do the practice.
