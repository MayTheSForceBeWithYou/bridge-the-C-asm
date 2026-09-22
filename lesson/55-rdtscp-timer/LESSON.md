# 55 — Timing with rdtscp

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

## Load-bearing facts for this machine

- Use **`rdtscp`**, not `rdtsc` (track standard; `constant_tsc` present).
- Timestamp in `%edx:%eax` (high:low); processor ID in `%ecx`.
- Relative comparisons only — variance is expected; print three trials.
- L1D line size on this track is **64 B** (used heavily in 59–60).

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` |
| run thrice | three positive deltas + `ok` |
| inspect helper asm | `rdtscp` mnemonic present |

## Assembling the 64-bit count

```c
return ((uint64_t)edx << 32) | (uint64_t)eax;
```

Declare `%ecx` as output or clobber. **Rejected wrong reading:** return only `%eax`
(truncates). **Rejected wrong reading:** use `rdtsc` because a blog said so.

## Zero delta sanity

If a delta is zero, the timed work was optimized away — check `-O` and `volatile` /
observable sinks.

## Navigation for Extended Asm

Search your `.s` / compiler asm dump for `rdtscp`. Confirm surrounding moves assemble
edx/eax into a 64-bit value.

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

## Worked timing protocol

1. Implement `rdtscp_now()` via Extended Asm or a tiny `.s`.
2. `t0 = rdtscp_now();` ~10M increments; `t1 = rdtscp_now();` print `t1-t0`.
3. Repeat thrice; note spread; print `ok`.

**Rejected wrong reading:** one sacred cycle count proves the forever cost of the loop.

## Extended Asm shape (illustrative)

Outputs for eax/edx; ecx as clobber or output; emit `rdtscp`; combine into `uint64_t`.
Match exercise 19 skills. Confirm with `objdump` that `rdtscp` appears — not `rdtsc`.

## Distinctions worth keeping straight

- **`rdtscp` vs `rdtsc`** — use `rdtscp` here.
- **Cycles vs nanoseconds** — you are not converting without a trusted frequency story.
- **Serializing timer vs fence soup** — `rdtscp` is the chosen tool; do not invent
  elaborate lfences unless you know why.
- **Optional `perf` (61)** — hardware counters later; this lesson is the portable-ish
  base.

## Deeper worked navigation (rdtscp timer)

- rdtscp required; edx:eax compose u64.
- Three trials; relative only.
- Zero delta ⇒ work deleted.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| helper asm | rdtscp mnemonic |
| three prints | positive deltas |
| variance | documented |

### Ordered navigation moves

1. Write helper.
2. Bracket loop.
3. Run thrice.
4. Sanity-check zeros.

### Rejected wrong readings (keep beside the artifact)

- rdtsc is an OK simplify.
- one run is enough.
- return only eax.

### Tool-line decoding reminders

- edx high eax low
- ecx id clobber
- relative vs absolute

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (55)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - used rdtsc
- returned only eax
- trusted one run as absolute

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

1. Why does this track forbid plain `rdtsc` in favor of `rdtscp`?
2. In which registers does the timestamp appear?
3. Why print three trials instead of one?
4. What does a zero delta likely indicate?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- `rdtscp` is the cycle timer for this machine and track.
- Relative comparisons beat absolute mythology.
- Variance across runs is expected; document it.
- Extended Asm (exercise 19 skills) is enough to call the instruction.

## Field-decoding recap for exercise 55

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc` (Extended Asm), `info gcc`, exercise 19

Now open `TASK.md` and do the practice.
