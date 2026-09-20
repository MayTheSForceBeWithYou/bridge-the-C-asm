# 18 — `volatile` vs optimization
<!-- concepts: c.p5.asm.volatile -->

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

## What the optimizer is allowed to assume

For a non-volatile local or global the compiler may assume that if *this*
thread/function did not write the object, and it did not call something that
could, the value is unchanged. A loop that writes a local then reads it without
escaping its address can collapse into "just use the final value" or disappear
if the final value is unused.

`volatile` disables that fantasy for that object: each volatile access in the
source should show up as memory access in the code, even at `-O2`.

## MMIO intuition (without a driver)

Memory-mapped I/O registers are locations where a store triggers hardware and a
load may return a new status bit every time. If the compiler merged two loads
into one, you would miss a status change. `volatile` is the C tool that models
"every access matters." You do not need real MMIO hardware for this lesson —
the asm difference is the point.

## Worked example

**The situation.** Both functions iterate a counter-like pattern with stores and
loads; only one uses `volatile`. You inspect `-O2` asm.

**Step 1 — non-volatile function.** Look for missing loops, immediate returns of
a constant, or empty bodies relative to source. The rejected wrong reading is:
"`-O2` broke my program because the loop is gone." If nothing observable escapes,
deleting the loop can be correct.

**Step 2 — volatile function.** Count loads/stores to the volatile object; they
should survive roughly in proportion to source accesses. The rejected wrong
reading is: "`volatile` means slow CPU instructions." It constrains *compiler*
elision; the opcodes are still ordinary moves.

**Step 3 — compare side by side.** Phrase the difference as "which memory
operations remain," not as "which binary is better."

**Step 4 — think MMIO.** One sentence on why duplicated volatile loads might be
required.

## Distinctions worth keeping straight

- **Observable side effect vs dead work** — optimizers delete the latter.
- **`volatile` access vs atomic / lock** — `volatile` is not a concurrency
  toolbox by itself.
- **Compiler obligation vs CPU cache reality** — lesson focuses on the compiler.
- **`-O0` keeps noise vs `-O2` reveals the rule** — use `-O2` for this contrast.

## Check yourself

1. Why can `-O2` delete a non-volatile store/load loop that "looks busy"?
2. What should remain in asm for a volatile access loop?
3. Why might MMIO-style code care about not merging loads?
4. Does `volatile` alone make a variable safe to share across threads without
   further synchronization?

## Key takeaways

- Without `volatile`, `-O2` may remove memory traffic that has no observable
  effect.
- With `volatile`, loads/stores to that object are preserved as accesses.
- That preservation models MMIO-like "every read/write matters."
- Prove it by diffing `-O2` listings of twin functions.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`
- `info gcc` — volatile notes after practice
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
