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

## Worked example

**The situation (perf present).** You paste a `perf stat` table into notes; shuffled
shows more branch-misses than sorted.

**Step 1 — detect.** `command -v perf || echo 'optional: sudo pacman -S perf'`. The
rejected wrong reading is: "I must install perf to pass Track F." It is optional; 55/58
already taught the mechanism.

**Step 2 — run.** `perf stat -e cycles,instructions,branches,branch-misses ./prog`.
Compare modes if available.

**Step 3 — fallback.** Without perf: rdtscp-only notes explicitly marked.

## Distinctions worth keeping straight

- **Optional tool vs required mechanism** — counters optional; timing concepts required.
- **`perf` not in REFS man list** — do not cite a required `man perf`.
- **External counters vs rdtscp** — different vantage points on similar truths.
- **Install hint vs mandatory sysadmin** — hint only; skip is valid.

## Check yourself

1. When is skipping this exercise correct?
2. Which exercise's phenomenon pairs best with branch-miss counters?
3. What do you write in notes if `perf` is absent?
4. Why is `man perf` not required reading on this track?

## Key takeaways

- `perf stat` can corroborate branch and cycle stories when available.
- Absence of `perf` is an expected, acceptable path.
- Never block learning on an optional package.
- `rdtscp` remains the baseline instrument.

## Lookup (not the lesson)

- exercises 55/58 — `perf help stat` only if installed; otherwise rdtscp fallback

Now open `TASK.md` and do the practice.
