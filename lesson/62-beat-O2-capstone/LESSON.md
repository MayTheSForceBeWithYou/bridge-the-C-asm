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

## Fair fight rules

| Rule | Meaning |
| ---- | ------- |
| Same `-O` | harness `make O=2` for both functions |
| Same input | identical data |
| Better structure | not "only my function gets `-O3`" |
| Repeat | win across several runs, not one fluke |

**Rejected wrong reading:** sprinkle `__attribute__((optimize(3)))` only on `fast_sum`.

## Mapping `make` to the capstone

| You type | Check afterward |
| -------- | --------------- |
| `make O=2` | `./prog` |
| run several times | `fast` cycles < `naive`; checksum/`ok` |

## Tactic menu from earlier exercises

| Idea | From |
| ---- | ---- |
| reduce mispredicts / branchless | 31, 58 |
| improve locality / blocking | 59 |
| avoid redundant work / closed form if fair | this lab |
| measure with `rdtscp` | 55 |

**Rejected wrong reading:** copy naive and hope `-O2` differs somehow.

## Warmup vs cheating

Brief warmup may be allowed by the harness — do not time only cold naive vs hot fast
beyond the rules.

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

## Worked approach sketch (no solution body)

1. Profile naive: branchy? strided? redundant?
2. Redesign structure (branchless, blocked, closed form if fair).
3. Keep harness `-O2`; same inputs; checksums.
4. Run ≥3 times; require consistent wins.

**Rejected wrong reading:** "trust `-O3` on the same shape."

## Distinctions worth keeping straight

- **Better algorithm vs higher -O on the same shape** — this lab wants the former.
- **Warmup vs cheating** — harness may allow brief warmup; do not time only cold naive
  vs hot fast unfairly beyond the rules.
- **Noise vs real wins** — repeat.
- **Capstone synthesis** — uses timers, branches, cache intuition from Track F.

## Deeper worked navigation (beat -O2)

- Better structure beats same-shape higher O.
- Same O=2, same inputs, repeatable win.
- Synthesize 55–60 tactics.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| naive cycles | higher |
| fast cycles | lower consistently |
| checksum | match |

### Ordered navigation moves

1. Understand naive.
2. Redesign.
3. Verify multi-run.

### Rejected wrong readings (keep beside the artifact)

- optimize attribute only on fast.
- copy naive and hope.
- one fluke win.

### Tool-line decoding reminders

- harness O=2
- warmup rules
- tactic menu

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (62)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - optimize attribute only on fast
- one lucky win
- same shape different O

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

1. What optimization level does the harness use for the comparison?
2. Why is "trust -O3 on the same source" not the intended solution?
3. Name two mechanism-level tactics from earlier exercises that might help.
4. How do you know you did not win by a single lucky timing?

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

- Structure can beat a compiler stuck optimizing a poor approach.
- Measure under the harness protocol, not vibes.
- Track F tools (`rdtscp`, branch/cache reasoning) are the means.
- Reproducible wins matter more than one fluke.

## Field-decoding recap for exercise 62

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- exercises 55–60, 31; `man 1 gcc`

Now open `TASK.md` and do the practice.
