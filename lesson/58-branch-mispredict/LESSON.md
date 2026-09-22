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

## Mapping `make` to the experiment

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` |
| run | cycle counts for sorted vs shuffled; matching checksum; `ok` |

## What stays outside the timed region

Sorting (if any) and data generation stay *outside* the `rdtscp` brackets. Timed work is
the threshold scan only.

**Rejected wrong reading:** "sorted is faster because sorting did the summing."

## Mechanism

Predictors love long runs of taken/not-taken (sorted around a threshold). Shuffled data
mispredicts often. Same multiset → same checksum; different control-pipeline cost.

**Rejected wrong reading:** "the compiler turned the sorted case into a closed formula."
Check asm if suspicious — usually both still loop.

## Tie to exercise 31

Branchy vs `cmov`/branchless codegen interacts with data pattern — you felt codegen in
31; here you feel mispredict cost with a clock.

## Worked example

**The situation.** Sorted timing is clearly lower than shuffled; checksums match; `ok`.

**Step 1 — same content, different order.** Sort a copy; do not change values. The
rejected wrong reading is: "sorted is faster because sorting does the summing work."
Sorting is outside the timed region; the timed loop only scans.

**Step 2 — time both.** Same `N`, same threshold, `rdtscp` brackets. The rejected wrong
reading is: "the compiler optimized the sorted case into a closed formula." Check asm if
suspicious — usually both still loop; the CPU prediction differs.

**Step 3 — explain.** Predictability / mispredict penalty; optional nod to `cmov`.

## Worked experiment checklist

| Step | Detail |
| ---- | ------ |
| Fill | random 0..255 |
| Copy + sort | one copy sorted; multiset identical |
| Time | only `sum_thresh` for each order |
| Check | checksums match; sorted cycles usually lower; `ok` |

Repeat if the gap is tiny. Tie the story to mispredicts; optional nod to `cmov` (31).

## Distinctions worth keeping straight

- **Algorithmic sort cost vs scan predictability** — do not time the sort in the
  comparison unless the harness says so.
- **Equal checksums** — proves same work product; times still differ.
- **Branchy IR vs cmov IR** — codegen choice interacts with data pattern.
- **One lucky run** — repeat if the gap is tiny; still expect sorted ≤ shuffled often.

## Deeper worked navigation (branch mispredict)

- Same multiset; order changes predict.
- Sort outside timed region.
- Checksums equal; times differ.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| sorted cycles | usually lower |
| shuffled cycles | higher |
| checksum | match |

### Ordered navigation moves

1. Build data.
2. Time both.
3. Explain predictability.
4. Tie to cmov lab.

### Rejected wrong readings (keep beside the artifact)

- sort did the summing.
- compiler closed-form sorted only.
- one lucky run enough.

### Tool-line decoding reminders

- rdtscp brackets
- equal work product
- mispredict story

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (58)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - timed the sort
- unequal checksums ignored
- one fluke gap

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

1. Why can identical values in different orders change cycle counts?
2. What must stay outside the timed region for a fair compare?
3. How does this connect to exercise 31?
4. What does a matching checksum buy you?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

Anchor drill: write the exact search string you used in the primary listing, then
quote one real line from your machine with every field labeled. If you cannot label
a field, that field is still unlearned — return to the decoding table above.

## Key takeaways

- Branch prediction makes data order a performance input.
- Sorted threshold scans are the classic demo.
- `rdtscp` quantifies what mispredict stories claim.
- Same answers, different times — performance is not only asymptotics.

## Field-decoding recap for exercise 58

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- exercise 31, exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
