# 23 — Stack scratch and 16-byte alignment

Registers are not enough once you need memory the caller cannot see. Locals live
in a stack frame you carve from `%rsp`, and the ABI demands that `%rsp` be
16-byte aligned *before* any `call`. This lesson connects prologue arithmetic to
that rule so your scratch space does not accidentally break the next call. By
the end you should decode negative `%rbp` offsets as slot addresses, predict
`%rsp` mod 16 after `call` / `push` / `sub`, and find spills in a disassembly
without guessing.

## What this lesson asks of you

Implement `long scratch_sum(long a, long b)` by storing both arguments into
stack locals, reloading them, and returning the sum. Even if you never call a
helper, practice a prologue that keeps alignment coherent the way real framed
code does.

`TASK.md` is the practice. This file teaches the recognition rules.

## Building a frame (AT&T shape)

A common `-O0` style prologue, read as a sequence of stack effects:

| Step | Instruction | What changes |
| ---- | ----------- | ------------ |
| 1 | `push %rbp` | Saves caller's frame pointer; `%rsp` decreases by 8 |
| 2 | `mov %rsp, %rbp` | Anchors locals at stable offsets from `%rbp` |
| 3 | `sub $N, %rsp` | Reserves `N` bytes for locals (and padding) |

Arguments still arrive in `%rdi` and `%rsi` (System V). You spill them to memory
with stores such as `movq %rdi, -8(%rbp)`, then reload with `movq` before adding
into `%rax`. The epilogue restores `%rsp` / `%rbp` (often `leave`, or
`mov %rbp, %rsp` then `pop %rbp`) and `ret`.

### Decoding a stack slot operand

AT&T memory form `offset(%rbp)` is a fielded address, not a blob:

| Token | Example | Meaning |
| ----- | ------- | ------- |
| Displacement | `-8` | Byte offset from the base register |
| Base | `%rbp` | Frame pointer after the prologue |
| Width suffix on `mov` | `movq` | Eight-byte (quad) transfer |

So `movq %rdi, -8(%rbp)` means: store the 64-bit value in `%rdi` into the
memory at `%rbp - 8`. A second local often sits at `-16(%rbp)`. Pick distinct
slots; overlapping them is a silent self-corruption bug.

A real listing line might look like:

```text
    117a:	48 89 7d f8          	mov    %rdi,-0x8(%rbp)
```

| Column | Example | Meaning |
| ------ | ------- | ------- |
| Address | `117a:` | Where this store instruction lives |
| Bytes | `48 89 7d f8` | Encoding (includes the displacement byte) |
| Mnemonic | `mov %rdi,-0x8(%rbp)` | Spill `a` into the first local slot |

**Rejected wrong reading:** "`-8(%rbp)` is an immediate constant I can `add`
into." It is an *address*. The value lives in memory at that address until you
load it back into a register.

**Rejected wrong reading:** "The `f8` in the raw bytes is the value of `a`."
That byte participates in encoding the displacement; `a`'s runtime value is
whatever was in `%rdi`, now stored in memory.

## Why 16-byte alignment bites

Walk the congruence class carefully — this is the recognition rule:

| Moment | `%rsp` mod 16 | Why |
| ------ | ------------- | --- |
| At the `call` in the caller | 0 | ABI requirement before `call` |
| First instruction of callee | 8 | `call` already pushed an 8-byte return address |
| After `push %rbp` | 0 | Another 8-byte push |
| After `sub $N, %rsp` | depends on `N` | `N` should keep you at 0 if you will `call` again |

If you `sub` an amount that is not a multiple of 16 after the push, you re-break
alignment before a nested `call`. Compilers therefore round the frame size so
that after the prologue, `%rsp` stays correctly aligned for outgoing calls.

Even without a nested call in this exercise, accounting for the `push` is the
habit that prevents mysterious crashes inside libc later (exercise 72).

**Rejected wrong reading:** "I can `sub $8` twice without thinking about total
frame alignment." Think in the final `%rsp` modulo 16, not in "how many locals
I feel like."

## Locals as addressed memory

A "local quad" is eight bytes on the stack. Two locals need at least 16 bytes
of reservation, plus any padding your alignment story requires. Addressing from
`%rbp` with negative offsets is the readable pattern matching Track A `-O0`
output; addressing from `%rsp` also works if you track the offsets carefully —
but every extra `push` shifts `%rsp`-relative addresses, while `%rbp`-relative
ones stay put after the prologue.

The point of spilling `a` and `b` even though they already sit in registers is
pedagogical: you practice stores and loads through the frame, not the fastest
code.

### What `make` maps to

| You type | Result | Inspect |
| -------- | ------ | ------- |
| `make && make run` | harness calls `scratch_sum` | stdout `ok` |
| `objdump -d` / `make disasm` | listing | search `<scratch_sum>:` |
| Compare to Track A `gcc -S -O0` | compiler prologue | same `push`/`mov`/`sub` shape |

## How to navigate the disassembly

1. Search for `<scratch_sum>:` (or your exact symbol name).
2. At the top of the function, identify `push %rbp` and `mov %rsp,%rbp` — that
   is frame setup, not the sum yet.
3. Find `mov` instructions with a memory operand involving `%rbp` — those are
   spills/reloads. Decode displacement vs base using the table above.
4. Confirm the sum lands in `%rax` before `ret` / `leave`.
5. Mentally add: entry skew 8 + each push 8 + `sub` immediate — check mod 16.

**Rejected wrong reading:** "Returning the value left in `%rdi` is fine." The
C caller reads `%rax`.

## Worked example

**The situation.** Stub `scratch.s` must become a framed function the harness
calls as `scratch_sum`.

**Step 1 — write a prologue and reserve space.** After `push %rbp` /
`mov %rsp, %rbp`, subtract enough for two quads (and padding if you plan a
`call`). Prefer a total `sub` that leaves `%rsp ≡ 0 (mod 16)` if you will nest
calls later. The rejected wrong reading is treating each `sub $8` as unrelated
to alignment.

**Step 2 — spill, reload, add.** Store `%rdi` and `%rsi` into distinct slots
(e.g. `-8(%rbp)` and `-16(%rbp)`), load them back into registers, add, leave
the sum in `%rax`. Mentally simulate `scratch_sum(3,4)`: memory at those slots
should hold 3 and 4 before the add. The rejected wrong reading is storing both
args to the same offset.

**Step 3 — epilogue and harness.** Restore the caller's `%rbp`/`%rsp` and
`ret`. `make && make run` should print `ok`. The rejected wrong reading is:
"if the sum is right I can skip restoring `%rbp`." Your caller may still be
using `%rbp`; trashing it fails later, not always inside your function.

**Step 4 — optional alignment audit.** From the listing, count pushes and
`sub` immediates between entry and any `call`. Apply the congruence table. If
you never `call`, still write the habit down — exercises 27 and 72 will grade it
with faults inside libc.

## Distinctions worth keeping straight

- **Alignment at `call` vs alignment on entry** — entry has the return address
  already pushed (`%rsp ≡ 8 mod 16`).
- **`%rbp` frame vs raw `%rsp` addressing** — same memory; different stability
  when you push more.
- **Displacement vs value** — `-8(%rbp)` is where; the quad stored there is what.
- **Instruction address vs slot displacement** — left column vs `-0x8` in the
  memory operand.
- **Reserving space vs pushing args** — outgoing stack args (rare for the first
  six integers) are separate from local scratch.
- **Needing a frame vs being required to use callee-saved regs** — orthogonal
  concerns.

## Check yourself

1. After `call` enters your function, what is `%rsp` mod 16 before your first
   instruction?
2. What does `push %rbp` do to that congruence?
3. Why might `sub $8, %rsp` after the push be a bad idea before a nested
   `call`?
4. In `movq %rdi, -8(%rbp)`, which part is the address and which register's
   value is being stored?
5. On a `mov %rdi,-0x8(%rbp)` `objdump` line, which token is the instruction
   address?
6. Where does the returned sum belong?
7. What search string finds your function in `objdump -d`?

## Key takeaways

- Stack locals are bytes you subtract from `%rsp` (often addressed via `%rbp`).
- ABI rule: `%rsp` 16-byte aligned at each `call`; account for the pushed return
  address and `push %rbp`.
- Decode `offset(%rbp)` as displacement + base — that is the slot address.
- Spill/reload practice is about memory traffic through the frame, not speed.
- Broken alignment often dies inside callees, not at your `sub` line.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Prologue patterns from Track A `gcc -S -O0` `.s` files
- `man 1 objdump` — listing flags when needed

Now open `TASK.md` and do the practice.
