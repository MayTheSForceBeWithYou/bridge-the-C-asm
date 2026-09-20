# 59 — Walking memory by cache lines
<!-- concepts: c.p5.perf.cache -->

Caches move data in *lines*, not bytes. On this machine the L1 data cache line size is
**64 bytes** (`getconf LEVEL1_DCACHE_LINESIZE` — given, do not rediscover as a puzzle).
Touching every byte sequentially uses each line fully; jumping 64 bytes per access
spends a whole line bring-in on one byte of intentional work. This lesson times that
difference.

## What this lesson asks of you

Allocate a large buffer (tens of MiB). Time sequential touches versus stride-64 touches
with the *same number of accesses*. Explain using the 64-byte line fact. Print both
cycle counts and `ok`.

## Why stride-64 hurts

Hardware fetches 64-byte lines. Sequential access amortizes that fetch across 64 bytes
of usefulness (for byte touches). A stride of 64 picks one byte per line (or one int per
line if you stride by 64 on a byte index), maximizing misses and bus/cache refill work
per useful touch.

Keep the access count equal so you compare policy, not iteration count. Use `rdtscp`
from exercise 55. Prevent the compiler from eliding touches (`volatile` accumulator or
similar).

## Worked example

**The situation.** Sequential cycles << stride-64 cycles for equal access counts; notes
cite 64-byte lines.

**Step 1 — big buffer.** Tens of MiB so you escape sitting entirely in hot L1 for the
whole pattern. The rejected wrong reading is: "a 4 KiB buffer is enough to see DRAM-like
effects." Too-small buffers may hide in cache and shrink the gap.

**Step 2 — equal accesses.** Sequential: `for i in 0..N: touch b[i]`. Stride: touch
`b[0], b[64], ...` wrapping or limiting so the *touch count* matches. The rejected wrong
reading is: "stride is slower because it executes more loop iterations." Equalize
iterations/accesses.

**Step 3 — explain with 64B lines.** Miss cost dominates.

## Distinctions worth keeping straight

- **Line size vs page size** — 64 B vs 4 KiB; different layers.
- **Equal work vs equal span** — match accesses; span may differ.
- **Prefetchers** — sequential gets hardware help; still expect a gap.
- **Stated 64 B fact** — use it; do not invent other sizes for this lab.

## Check yourself

1. What is this machine's L1D line size for the track?
2. Why hold the number of touches constant?
3. Why use a large buffer?
4. How do you stop DCE from deleting the touches?

## Key takeaways

- Cache lines make access *pattern* matter as much as access *count*.
- 64-byte stride is a hostile pattern on this CPU's L1 line size.
- Sequential scans amortize line fills.
- Measure with `rdtscp`; explain with the line-size mechanism.

## Lookup (not the lesson)

- exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
