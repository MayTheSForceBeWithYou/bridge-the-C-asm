# 18 — `volatile` vs optimization

`volatile` tells the compiler that reads and writes to an object have
side effects it must not invent away — each abstract-machine access should
become a real load or store (as far as the compiler's obligations go). Ordinary
objects may have dead stores deleted, loops collapsed to constants, or loads
hoisted when the compiler proves nothing else could change the value. By the end
of this lesson you should see `-O2` erase a non-volatile busy loop's memory
traffic while keeping the volatile twin's loads/stores, and say why that matters
for MMIO-shaped reasoning.

## What this lesson asks of you

Write two functions with similar store/load loops — one on a plain `int`, one on
`volatile int`. Build with `make O=2 asm disasm` and compare. Practice in
`TASK.md`.

## What `make` is naming for you

| You type | Why this lesson needs it |
| -------- | ------------------------ |
| `make O=2 asm` | The contrast appears at `-O2`; `-O0` keeps both "busy" |
| `make O=2 disasm` | Confirm in `prog.lst` — search `<plain>:` / `<vola>:` |
| Avoid relying only on `-O0` | You will not see the elision story |

## What the optimizer is allowed to assume

For a non-volatile local or global the compiler may assume that if *this*
thread/function did not write the object, and it did not call something that
could, the value is unchanged. A loop that writes a local then reads it without
escaping its address can collapse into "just use the final value" or disappear
if the final value is unused.

`volatile` disables that fantasy for that object: each volatile access in the
source should show up as memory access in the code, even at `-O2`.

### Real `-O2` contrast (shape)

A plain loop that only leaves a final integer may become:

```text
plain:
        xorl    %edx, %edx
        leal    -1(%rdi), %eax
        testl   %edi, %edi
        cmovle  %edx, %eax
        ret
```

No per-iteration store — the compiler computed the mathematical result.

A volatile twin still shows memory traffic:

```text
vola:
        movl    $0, -4(%rsp)     # volatile store
        …
.L7:
        movl    %eax, -4(%rsp)   # volatile store each trip
        …
        movl    -4(%rsp), %eax   # volatile load before return
        ret
```

| Function | What remains at `-O2` | Reading |
| -------- | --------------------- | ------- |
| plain | closed form / few ops | loop elided — OK if result matches |
| volatile | repeated `movl` to/from a stack slot | accesses preserved |

### Navigation

1. Build **only** at `-O2` for the main contrast (`make O=2 asm`).
2. Search `plain:` — count memory operands to the loop variable.
3. Search `vola:` — count `movl` to `-N(%rsp)` or `-N(%rbp)`.
4. Phrase the difference as "which memory operations remain."

**Rejected wrong reading:** "`-O2` broke my program because the loop is gone."
If nothing observable escapes, deleting the loop can be correct.

**Rejected wrong reading:** "`volatile` means slow CPU instructions." It
constrains *compiler* elision; the opcodes are still ordinary moves.

## MMIO intuition (without a driver)

Memory-mapped I/O registers are locations where a store triggers hardware and a
load may return a new status bit every time. If the compiler merged two loads
into one, you would miss a status change. `volatile` is the C tool that models
"every access matters." You do not need real MMIO hardware for this lesson —
the asm difference is the point.

One sentence to keep: "volatile tells the compiler not to delete or coalesce
accesses; it does not by itself make cross-thread sharing safe."

## What `volatile` is not

| Claim | Verdict |
| ----- | ------- |
| "`volatile` is a mutex" | False — use atomics / locks for concurrency |
| "`volatile` fixes data races" | False |
| "`volatile` forces every access to DRAM past the cache" | Not what C guarantees; this lesson is about the compiler |
| "`volatile` preserves MMIO-shaped access counts" | The intended model here |

## Worked example

**The situation.** Both functions iterate a counter-like pattern with stores and
loads; only one uses `volatile`. You inspect `-O2` asm.

**Step 1 — non-volatile function.** Look for missing loops, immediate returns of
a constant, or empty bodies relative to source.

**Step 2 — volatile function.** Count loads/stores to the volatile object; they
should survive roughly in proportion to source accesses.

**Step 3 — compare side by side.** Phrase as "which memory operations remain."

**Step 4 — think MMIO.** One sentence on why duplicated volatile loads might be
required.


## Counting accesses without fooling yourself

In `vola` at `-O2`, every `movl …, -4(%rsp)` (or similar) that corresponds to a
source assignment counts as a surviving store. Do not count prologue noise or
unrelated spills. Compare that count to the number of iterations you asked for
(or note if the compiler unrolled and doubled stores per iteration — still
volatile traffic, just scheduled differently).

For `plain`, ask: is there *any* per-iteration store to the loop variable? If
not, elision happened.


## Suggested twin bodies (shape, not a solution dump)

Keep both functions structurally parallel: same loop bounds, same pattern of
store then (maybe) load. Only the type qualifier should differ. That way a
side-by-side diff of `.s` files attributes differences to `volatile`, not to
accidental algorithmic drift.


## One-sentence takeaway card

Write and keep: "At `-O2`, non-volatile dead memory loops may vanish; volatile
accesses remain as real loads/stores so MMIO-shaped code sees every access."


## Rejected wrong reading (threads)

"**I marked the flag `volatile`, so my worker thread and main thread can share it
without atomics.**" `volatile` does not provide inter-thread synchronization.
Use it here for access preservation / MMIO-shaped reasoning, not as a lock.

## Distinctions worth keeping straight

- **Observable side effect vs dead work** — optimizers delete the latter.
- **`volatile` access vs atomic / lock** — `volatile` is not a concurrency
  toolbox by itself.
- **Compiler obligation vs CPU cache reality** — lesson focuses on the compiler.
- **`-O0` keeps noise vs `-O2` reveals the rule** — use `-O2` for this contrast.
- **Elided loop vs wrong answer** — check the returned value before crying bug.

## Check yourself

1. Why can `-O2` delete a non-volatile store/load loop that "looks busy"?
2. What should remain in asm for a volatile access loop?
3. Why might MMIO-style code care about not merging loads?
4. Does `volatile` alone make a variable safe to share across threads without
   further synchronization?
5. Why is `make asm` at default `-O0` the wrong primary build for this lesson?
6. How do you count "surviving accesses" in `vola` from the listing?

## Key takeaways

- Without `volatile`, `-O2` may remove memory traffic that has no observable
  effect.
- With `volatile`, loads/stores to that object are preserved as accesses.
- That preservation models MMIO-like "every read/write matters."
- Prove it by diffing `-O2` listings of twin functions — cite surviving `movl`s.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`
- `info gcc` — volatile notes after practice
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
