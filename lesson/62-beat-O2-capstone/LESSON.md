# 62 — Beat the compiler's -O2 of a naive loop

Sometimes the compiler's best effort on *bad* source still loses to a human who changes
the algorithm: branchless form, blocking for cache, precomputation, different data
layout. This capstone gives a naive, hostile `naive_sum` measured at `-O2`, and asks
your `fast_sum` to win on the same input under the harness's `rdtscp` protocol.

## What this lesson asks of you

Read `naive_sum` without "optimizing the compiler." Measure it at `-O2`. Implement
`fast_sum` with a structurally better approach. `./prog` must print both cycle counts
and `ok` when fast wins (small warmup allowed). Re-run a few times so you did not win by
noise.

## Fair fight rules

- Same optimization level for the binary the harness builds (`make O=2`).
- Same input data for both functions.
- Do not disable the naive function's optimization with tricks; beat it by better
  method.
- Ideas from 55–60 and 31: reduce mispredicts, improve locality, avoid redundant work.

Winning by reading `naive_sum` and writing an equivalent closed form is fair if the
harness input allows it — the point is intentional structure, not opaque pragma magic.

## Worked example

**The situation.** Naive burns cycles on a branch-heavy scan; your version uses a
branchless or blocked approach; harness prints `ok`.

**Step 1 — understand naive.** Why is it slow (branches? stride? dependency)? The
rejected wrong reading is: "I will sprinkle `__attribute__((optimize(3)))` only on my
function and call it a day." That dodges the lesson.

**Step 2 — redesign.** Implement clear fast path. The rejected wrong reading is: "copy
naive and hope `-O2` differs somehow." Same shape yields same speed.

**Step 3 — verify stability.** Several runs; fast consistently ahead; checksums still
match if required.

## Distinctions worth keeping straight

- **Better algorithm vs higher -O on the same shape** — this lab wants the former.
- **Warmup vs cheating** — harness may allow brief warmup; do not time only cold naive
  vs hot fast unfairly beyond the rules.
- **Noise vs real wins** — repeat.
- **Capstone synthesis** — uses timers, branches, cache intuition from Track F.

## Check yourself

1. What optimization level does the harness use for the comparison?
2. Why is "trust -O3 on the same source" not the intended solution?
3. Name two mechanism-level tactics from earlier exercises that might help.
4. How do you know you did not win by a single lucky timing?

## Key takeaways

- Structure can beat a compiler stuck optimizing a poor approach.
- Measure under the harness protocol, not vibes.
- Track F tools (`rdtscp`, branch/cache reasoning) are the means.
- Reproducible wins matter more than one fluke.

## Lookup (not the lesson)

- exercises 55–60, 31; `man 1 gcc`

Now open `TASK.md` and do the practice.
