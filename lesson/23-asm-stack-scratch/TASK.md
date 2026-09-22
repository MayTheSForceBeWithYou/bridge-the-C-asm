# 23 — Stack scratch + 16-byte align (practice)

Read `LESSON.md` first — especially the alignment congruence table and how to
decode `offset(%rbp)`. This file is only the lab.

## Build and run

1. Implement `long scratch_sum(long a, long b)` in `scratch.s`: allocate space
   for at least two local quads, store `a` and `b`, reload, return `a+b`.
2. Keep frame alignment coherent (`push %rbp` changes `%rsp` — account for it).
3. `make && make run`.

## Done when

- Harness prints `ok`.
- You can point at two distinct stack slots in your listing and say what each
  holds after the spills.

## Lookup

`man 1 as`, `info as`; Track A prologue patterns. Optional: dap memory-view of
`$rsp` — see `NVIM_DAP.md`.
