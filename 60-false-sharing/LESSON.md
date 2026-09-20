# 60 — False sharing between threads
<!-- concepts: c.p5.perf.sharing -->

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

## Distinctions worth keeping straight

- **False vs true sharing** — different objects on one line vs intentional shared state.
- **64-byte line fact** — same number as exercise 59, new phenomenon.
- **Correctness vs speed** — both layouts can be correct; times differ.
- **Atomics** — not required for the classic demo if each thread owns one counter; still
  avoid data races on the *same* int.

## Check yourself

1. Why can distinct variables still contend?
2. What padding goal stops false sharing here?
3. Which library flag links pthreads cleanly on this box?
4. How is this different from the stride lesson?

## Key takeaways

- False sharing is a coherence problem, not a logic bug.
- Align/pad hot per-thread counters to line boundaries.
- pthreads + `rdtscp` make it measurable.
- L1 line size (64 B) is the critical constant again.

## Lookup (not the lesson)

- `man 3 pthread_create`, exercise 55, `man 1 gcc`

Now open `TASK.md` and do the practice.
