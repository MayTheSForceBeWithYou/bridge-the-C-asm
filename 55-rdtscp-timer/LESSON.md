# 55 — Timing with rdtscp
<!-- concepts: c.p5.perf.rdtscp -->

Wall clocks and `clock()` are coarse for tiny loops. The CPU's timestamp counter lets
you count cycles between two points. On this machine you must use **`rdtscp`** (not
plain `rdtsc`): it is available, and `constant_tsc` is present so counts are usable for
*relative* comparisons. Absolute "this always takes N cycles" claims are still wrong —
variance is real.

## What this lesson asks of you

Implement `rdtscp_now()` (Extended Asm or a tiny `.s`), time ~10M simple increments,
print elapsed cycles thrice, and print `ok`. Treat three runs as a small ensemble, not
one sacred number.

## Why rdtscp, not rdtsc

`rdtsc` reads the timestamp counter but does not serialize as strongly; out-of-order
execution can move the read relative to nearby work. `rdtscp` reads the counter and a
processor ID, with stronger ordering properties that make "start/end around this loop"
more honest for lab purposes. This track standardizes on `rdtscp` because REFS.md
verified it here — do not "simplify" to `rdtsc`.

The instruction returns a 64-bit value split across `%edx:%eax` (high:low), plus
`%ecx` for the ID you can ignore for basic timing. Your helper should assemble those
halves into a `uint64_t`.

## Relative comparisons only

Same binary, same machine, quiet-ish system: compare loop A vs loop B, or run-to-run
spread. Do not publish absolute cycle counts as truths across machines or boots.
Warmup, interrupts, turbo, and frequency policies all move the numbers.

## Worked example

**The situation.** `./prog` prints three positive deltas and `ok`.

**Step 1 — helper.** Extended Asm: `rdtscp` with outputs for eax/edx (and ecx clobber
or output). Combine `((uint64_t)edx << 32) | eax`. The rejected wrong reading is: "I
used `rdtsc` because Stack Overflow said it is fine." On this track, `rdtscp` is the
binding fact.

**Step 2 — bracket the loop.** `t0 = rdtscp_now();` work; `t1 = rdtscp_now();` print
`t1 - t0`. The rejected wrong reading is: "one measurement proves the loop takes exactly
this many cycles every time." Run thrice; note variance.

**Step 3 — sanity.** Deltas positive and roughly same order of magnitude; if one is
zero, your asm or loop was optimized away — check `-O` and `volatile` needs.

## Distinctions worth keeping straight

- **`rdtscp` vs `rdtsc`** — use `rdtscp` here.
- **Cycles vs nanoseconds** — you are not converting without a trusted frequency story.
- **Serializing timer vs fence soup** — `rdtscp` is the chosen tool; do not invent
  elaborate lfences unless you know why.
- **Optional `perf` (61)** — hardware counters later; this lesson is the portable-ish
  base.

## Check yourself

1. Why does this track forbid plain `rdtsc` in favor of `rdtscp`?
2. In which registers does the timestamp appear?
3. Why print three trials instead of one?
4. What does a zero delta likely indicate?

## Key takeaways

- `rdtscp` is the cycle timer for this machine and track.
- Relative comparisons beat absolute mythology.
- Variance across runs is expected; document it.
- Extended Asm (exercise 19 skills) is enough to call the instruction.

## Lookup (not the lesson)

- `man 1 gcc` (Extended Asm), `info gcc`, exercise 19

Now open `TASK.md` and do the practice.
