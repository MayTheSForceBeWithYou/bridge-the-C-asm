# 59 — Walking memory by cache lines

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

## Load-bearing fact

L1 data cache line size on this machine is **64 bytes**
(`getconf LEVEL1_DCACHE_LINESIZE`). Use it; do not invent another size for this lab.

## Mapping `make` to the experiment

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` |
| run | sequential cycles << stride-64 cycles; `ok` |

## Equalize access counts

Sequential: `N` touches of adjacent bytes/elements.
Stride-64: `N` touches at indices `0, 64, 128, …` (wrap/limit as needed).

**Rejected wrong reading:** "stride is slower because it executed more iterations."

## Why a large buffer

Tens of MiB so the pattern cannot sit entirely in hot L1 for the whole run.
**Rejected wrong reading:** "4 KiB is enough to see DRAM-like effects."

## Line size vs page size

64 B lines vs 4 KiB pages — different layers. Prefetchers help sequential; still expect a
gap.

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

## Worked buffer sizing and touch loop

Allocate tens of MiB. Time `N` sequential touches vs `N` stride-64 touches with `rdtscp`.
Prevent DCE via `volatile` accumulator.

Explain with the **64-byte** line fact: sequential amortizes a line fill; stride-64 pays
a fill per useful touch.

**Rejected wrong reading:** confuse 64 B lines with 4 KiB pages.

## Distinctions worth keeping straight

- **Line size vs page size** — 64 B vs 4 KiB; different layers.
- **Equal work vs equal span** — match accesses; span may differ.
- **Prefetchers** — sequential gets hardware help; still expect a gap.
- **Stated 64 B fact** — use it; do not invent other sizes for this lab.

## Deeper worked navigation (cache stride)

- L1D line = 64 B here.
- Equalize access counts.
- Large buffer; prevent DCE.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| seq cycles | << stride |
| stride-64 | one useful byte per line |
| N touches | matched |

### Ordered navigation moves

1. Alloc big.
2. Time seq vs stride.
3. Explain with 64 B.

### Rejected wrong readings (keep beside the artifact)

- 4 KiB buffer enough.
- stride slower due to more iterations.
- invent another line size.

### Tool-line decoding reminders

- 64 B fact
- line vs page
- volatile sink

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (59)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - tiny buffer
- unequal touch counts
- wrong line size invented

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

1. What is this machine's L1D line size for the track?
2. Why hold the number of touches constant?
3. Why use a large buffer?
4. How do you stop DCE from deleting the touches?

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

- Cache lines make access *pattern* matter as much as access *count*.
- 64-byte stride is a hostile pattern on this CPU's L1 line size.
- Sequential scans amortize line fills.
- Measure with `rdtscp`; explain with the line-size mechanism.

## Field-decoding recap for exercise 59

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
