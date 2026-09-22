# 60 — False sharing between threads

Two threads writing *different* variables can still thrash if those variables share a
cache line. Coherence traffic bounces the line between cores — *false* sharing, because
the program never intended a shared variable. Padding so each counter owns its own
64-byte line removes the accidental sharing. This lesson times both layouts with
pthreads and `rdtscp`.

## What this lesson asks of you

Spawn two threads that each increment their own counter many times. Layout A: adjacent
`int`s in one struct (same line). Layout B: pad to 64-byte ownership per counter. Link
with `-lpthread`. Compare wall/cycle times for the paired run; print both and `ok`.

## Coherence in one paragraph

Each core's cache holds lines. When core 1 writes a line that core 0 also caches for
its "own" adjacent int, protocols invalidate or bounce ownership. Separating counters by
≥64 bytes (this machine's line size) stops that ping-pong for the hot fields.

## Load-bearing fact

Same **64-byte** L1 line size as exercise 59 — new phenomenon: coherence traffic between
cores.

## Mapping `make` to the experiment

| You type | Check afterward |
| -------- | --------------- |
| `make` (link `-lpthread`) | `./prog` |
| run | padded layout faster than adjacent; correct counts; `ok` |

## Adjacent vs padded layouts

| Layout | Shape | Effect |
| ------ | ----- | ------ |
| Adjacent | `struct { int a; int b; }` | both counters likely one line → false sharing |
| Padded | each counter owns ≥64 B | coherence ping-pong stops for those fields |

**Rejected wrong reading:** "false sharing only happens when both threads write the *same*
variable." Same line is enough.

**Rejected wrong reading:** "any 1-byte pad fixes it." You need line separation.

## Correctness vs speed

Both layouts can be logically correct; times differ. Each thread must own its own
counter (no data race on the same `int`).

## Worked example

**The situation.** Padded timing beats adjacent timing; both produce correct final
counts; `ok`.

**Step 1 — adjacent struct.** `struct { int a; int b; }` — thread0 bumps `a`, thread1
bumps `b`. The rejected wrong reading is: "false sharing only happens when both threads
write the *same* variable." Same line is enough.

**Step 2 — padded struct.** Align or insert `char pad[64 - sizeof(int)]` (careful with
sizes) so each counter starts on its own line. The rejected wrong reading is: "any
padding of 1 byte fixes it." You need line separation, not token padding.

**Step 3 — time the pair.** Barrier or join so you measure both threads' work window.
`-lpthread` via Makefile `LDLIBS`.

## Padding recipe (line ownership)

Give each counter its own 64-byte slot (`alignas(64)` or explicit `char pad[64 -
sizeof(int)]` carefully). Thread 0 bumps `a`; thread 1 bumps `b`; join; compare times.

Link with `-lpthread` (`LDLIBS` in the Makefile). **Rejected wrong reading:** atomics are
required for the classic demo when each thread owns a distinct counter — still avoid races
on the *same* int.

## Distinctions worth keeping straight

- **False vs true sharing** — different objects on one line vs intentional shared state.
- **64-byte line fact** — same number as exercise 59, new phenomenon.
- **Correctness vs speed** — both layouts can be correct; times differ.
- **Atomics** — not required for the classic demo if each thread owns one counter; still
  avoid data races on the *same* int.

## Deeper worked navigation (false sharing)

- Different vars; same line; coherence tax.
- Pad to 64 B ownership.
- Correctness ≠ speed.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| adjacent time | slower |
| padded time | faster |
| counts | correct |

### Ordered navigation moves

1. Adjacent struct.
2. Padded struct.
3. pthread + rdtscp.
4. Compare.

### Rejected wrong readings (keep beside the artifact)

- need same variable.
- 1-byte pad fixes.
- atomics always required for demo.

### Tool-line decoding reminders

- 64 B again
- -lpthread
- join/barrier window

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (60)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - 1-byte padding
- thought need same variable
- forgot -lpthread

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

1. Why can distinct variables still contend?
2. What padding goal stops false sharing here?
3. Which library flag links pthreads cleanly on this box?
4. How is this different from the stride lesson?

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

- False sharing is a coherence problem, not a logic bug.
- Align/pad hot per-thread counters to line boundaries.
- pthreads + `rdtscp` make it measurable.
- L1 line size (64 B) is the critical constant again.

## Field-decoding recap for exercise 60

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 3 pthread_create`, exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
