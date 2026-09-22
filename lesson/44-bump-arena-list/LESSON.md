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

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` prints `ok` |
| `grep -n malloc prog.c` | no malloc on your node path |
| optional prints of node pointers | addresses fall inside `[base, base+cap)` |

## Arena state and `arena_alloc` steps

| State | Role |
| ----- | ---- |
| `base` | start of mapped region |
| `cursor` / `used` | next free offset |
| `capacity` | mapped bytes |

Allocation: round `n` up for alignment → fail if `cursor+n > capacity` → return
`base+cursor` → advance cursor.

## Alignment (recognition rule)

Align to at least `_Alignof(Node)` (commonly 8). **Rejected wrong reading:** "alignment
does not matter on x86-64." Sanitizers and other ISAs disagree; so does good hygiene.

## Verifying "no malloc for nodes"

| Check | Success looks like |
| ----- | ------------------ |
| `grep malloc prog.c` | no hits for your node allocator |
| node addresses | inside the mmap span |
| `nm` showing undefined `malloc` | may still happen via `printf` — judge *your* calls |

**Rejected wrong reading:** "`nm` shows `malloc`, so I failed." Libc for `printf` can
leave `malloc` unresolved at link time without your nodes using it.

## Do not `free` arena pointers

`free` on a non-`malloc` pointer is undefined. Lifetime ends with `munmap` (or cursor
reset). **Rejected wrong reading:** "`free` each node to be tidy."

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

## Worked walk-through of three nodes

1. `mmap` 64 KiB RW anonymous; set `cursor = 0`.
2. `n1 = arena_alloc(sizeof *n1);` fill `val = 1`.
3. Likewise `n2`/`n3` with values 2 and 3; link `n1→n2→n3→NULL`.
4. Walk and sum; require 6; print `ok`; `munmap`.

**Rejected wrong reading:** bump out of a 64 KiB stack array — stack space is limited and
ABI-fragile; this lab wants an explicit mapped arena.

## Capacity failure mode

If `cursor + need > capacity`, return NULL / abort as your API dictates. Silent wrap into
unmapped memory is the classic arena bug.

## Distinctions worth keeping straight

- **Bump free vs malloc free** — arena frees all or nothing; malloc frees per object.
- **Arena pointer vs heap pointer** — same VA usability; different lifetime protocol.
- **printf's malloc vs yours** — libc internals may allocate; your nodes must not.
- **Alignment** — bump without round-up is a classic bug.

## Deeper worked navigation (bump arena)

- State = base, cursor, capacity.
- Align allocations.
- Never free arena pointers with free(3).

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| mmap span | all node addresses inside |
| grep malloc | no node path |
| sum | 1+2+3=6 |

### Ordered navigation moves

1. mmap arena.
2. alloc three nodes aligned.
3. link+sum.
4. munmap.

### Rejected wrong readings (keep beside the artifact)

- Stack buffer is an OK arena.
- nm showing malloc always means you cheated.
- free(node) is tidy.

### Tool-line decoding reminders

- alignment round-up
- capacity check before advance
- lifetime = whole arena

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (44)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - forgot alignment
- free(arena_ptr)
- arena on huge stack array

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

1. What state does a bump allocator keep, and what does "free" mean for it?
2. Why unmap the arena instead of freeing each node?
3. How do you verify you did not call `malloc` for nodes?
4. What goes wrong if `arena_alloc` forgets alignment?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Bump arenas trade flexible free for speed and simplicity.
- `mmap` (+ `munmap`) is enough backing store for a lab arena.
- Lifetime grouping (all nodes die together) matches arena semantics.
- Alignment and capacity checks are part of a correct bump API.

## Field-decoding recap for exercise 44

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 mmap`, `man 2 munmap`, `man 3 printf`

Now open `TASK.md` and do the practice.
