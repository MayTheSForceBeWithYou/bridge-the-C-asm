# 58 — When sorted data makes branches cheap

A loop that branches on `a[i] > threshold` behaves differently on sorted versus
shuffled data even when the *element multiset* is identical. CPUs predict branches;
sorted runs of taken/not-taken are easy, random patterns are not. This lesson times both
with `rdtscp` and ties the gap to exercise 31's `cmov` versus real branches theme.

## What this lesson asks of you

Fill an array with random 0..255 values, copy it, sort one copy, time `sum_thresh` on
both, print cycle counts, verify checksum/`ok`. Explain the gap via predictability.

## Mechanism

Branch predictors learn patterns. Long runs of "yes" then long runs of "no" (sorted
data around a threshold) mispredict mainly near the transition. Shuffled data
mispredicts frequently. The arithmetic is the same; the control pipeline cost is not.

Branchless techniques (`cmov`, masks) can flatten that gap — you saw the codegen side
in exercise 31; here you *feel* it with a clock.

## Worked example

**The situation.** Sorted timing is clearly lower than shuffled; checksums match; `ok`.

**Step 1 — same content, different order.** Sort a copy; do not change values. The
rejected wrong reading is: "sorted is faster because sorting does the summing work."
Sorting is outside the timed region; the timed loop only scans.

**Step 2 — time both.** Same `N`, same threshold, `rdtscp` brackets. The rejected wrong
reading is: "the compiler optimized the sorted case into a closed formula." Check asm if
suspicious — usually both still loop; the CPU prediction differs.

**Step 3 — explain.** Predictability / mispredict penalty; optional nod to `cmov`.

## Distinctions worth keeping straight

- **Algorithmic sort cost vs scan predictability** — do not time the sort in the
  comparison unless the harness says so.
- **Equal checksums** — proves same work product; times still differ.
- **Branchy IR vs cmov IR** — codegen choice interacts with data pattern.
- **One lucky run** — repeat if the gap is tiny; still expect sorted ≤ shuffled often.

## Check yourself

1. Why can identical values in different orders change cycle counts?
2. What must stay outside the timed region for a fair compare?
3. How does this connect to exercise 31?
4. What does a matching checksum buy you?

## Key takeaways

- Branch prediction makes data order a performance input.
- Sorted threshold scans are the classic demo.
- `rdtscp` quantifies what mispredict stories claim.
- Same answers, different times — performance is not only asymptotics.

## Lookup (not the lesson)

- exercise 31, exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
