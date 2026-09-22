# 61 — Optional hardware counters with perf

`rdtscp` counts cycles from inside the process. `perf stat` (when installed) samples
CPU performance counters from the outside — cycles, instructions, branches,
branch-misses — without you wiring each event by hand. This lesson is **optional**: if
`perf` is missing, skip with the pacman hint and rely on `rdtscp` from exercise 55.
There is no `man perf` in REFS.md; use `perf help stat` only if the tool exists.

## What this lesson asks of you

Check for `perf`. If present, run `perf stat` on a binary (exercise 58's sorted vs
shuffled modes are ideal) and record the counter table. If absent, still have a simple
`prog` loop timed with `rdtscp` and note "perf skipped" in your notes.

## What branch-misses add

Exercise 58 claimed mispredicts drive the sorted/shuffled gap. `perf` can show
`branch-misses` rising on the shuffled run — independent evidence. Cycles and
instructions roughly cross-check your internal timer.

Permissions or paranoid settings sometimes block counters; if `perf` errors, document
that and fall back — do not fight the kernel as part of this track.

## Optional tool policy

If `command -v perf` fails, skip with the pacman hint and keep `rdtscp` notes marked
"perf skipped". **Rejected wrong reading:** "I must install perf to pass Track F."

## Mapping commands when `perf` exists

```bash
perf stat -e cycles,instructions,branches,branch-misses ./prog
```

Prefer exercise 58's sorted vs shuffled modes if available.

## Decoding `perf stat` columns

| Counter | Meaning |
| ------- | ------- |
| `cycles` | CPU cycles |
| `instructions` | retired instructions |
| `branches` | branches |
| `branch-misses` | mispredicts — should rise on shuffled vs sorted |

**Navigation:** read the table at the end of `perf stat` output; compare two runs.

## Permissions / paranoid settings

If `perf` errors, document and fall back — do not fight the kernel as part of this track.

## `man perf` is not required reading here

Use `perf help stat` only if installed; REFS does not mandate `man perf`.

## Worked example

**The situation (perf present).** You paste a `perf stat` table into notes; shuffled
shows more branch-misses than sorted.

**Step 1 — detect.** `command -v perf || echo 'optional: sudo pacman -S perf'`. The
rejected wrong reading is: "I must install perf to pass Track F." It is optional; 55/58
already taught the mechanism.

**Step 2 — run.** `perf stat -e cycles,instructions,branches,branch-misses ./prog`.
Compare modes if available.

**Step 3 — fallback.** Without perf: rdtscp-only notes explicitly marked.

## Worked notes template

```text
perf present?: yes/no
if no: pacman hint noted; rdtscp fallback from 55/58
if yes: paste perf stat table for sorted vs shuffled
branch-misses sorted: ____
branch-misses shuffled: ____
```

That is enough. Do not block Track F on an optional package.

## Distinctions worth keeping straight

- **Optional tool vs required mechanism** — counters optional; timing concepts required.
- **`perf` not in REFS man list** — do not cite a required `man perf`.
- **External counters vs rdtscp** — different vantage points on similar truths.
- **Install hint vs mandatory sysadmin** — hint only; skip is valid.

## Deeper worked navigation (optional perf)

- Optional; rdtscp remains baseline.
- branch-misses corroborates 58.
- Missing perf is an OK path.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| command -v perf | present/absent |
| perf stat table | if present |
| notes | perf skipped if absent |

### Ordered navigation moves

1. Detect.
2. Run or skip.
3. Record.

### Rejected wrong readings (keep beside the artifact)

- must install to pass Track F.
- man perf is required.
- block on permissions forever.

### Tool-line decoding reminders

- cycles/instr/branches/misses columns
- pacman hint
- fallback wording

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (61)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - blocked Track F on missing perf
- no notes when skipping
- ignored branch-misses columns

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

1. When is skipping this exercise correct?
2. Which exercise's phenomenon pairs best with branch-miss counters?
3. What do you write in notes if `perf` is absent?
4. Why is `man perf` not required reading on this track?

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

- `perf stat` can corroborate branch and cycle stories when available.
- Absence of `perf` is an expected, acceptable path.
- Never block learning on an optional package.
- `rdtscp` remains the baseline instrument.

## Field-decoding recap for exercise 61

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- exercises 55/58 — `perf help stat` only if installed; otherwise rdtscp fallback

Now open `TASK.md` and do the practice.
