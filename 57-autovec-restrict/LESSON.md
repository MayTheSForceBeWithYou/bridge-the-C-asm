# 57 — Autovectorization and the aliasing hazard
<!-- concepts: c.p5.opt.autovec -->

GCC at `-O3 -march=native` can emit packed SIMD operations for simple array loops —
when it can prove (or is told) that pointers do not alias in dangerous ways. `restrict`
is how you promise non-aliasing. This lesson shows one loop that vectorizes, one that
does not until fixed, using `-fopt-info-vec-optimized` (available on the GCC here).

## What this lesson asks of you

Implement `add_arrays` (clear non-aliasing), `add_hazard` (parameters that could alias),
and `add_restrict` with `restrict` pointers. Build with opt-info redirected to
`vec.txt`, confirm which loops report vectorization, and peek at packed ops in
`objdump`.

## Why aliasing blocks vectorization

If `dst` and `src` might overlap, transforming `dst[i] += src[i]` into wide loads/stores
can change semantics. Compilers stay conservative. When you pass disjoint arrays and
mark `restrict`, you accept responsibility for that promise.

`-fopt-info-vec-optimized` prints which loops were vectorized — read that file; do not
guess.

## What success looks like in asm

Look for packed arithmetic (`paddd`, `vaddps`, similar) on the hot path for the safe
loops. The hazard function should remain scalar until you fix the contract.

## Worked example

**The situation.** `vec.txt` mentions `add_arrays` / `add_restrict` but not the hazard
version; `./prog` prints `ok`.

**Step 1 — three functions.** Same math, different pointer contracts. The rejected
wrong reading is: "renaming variables is enough for the compiler to vectorize overlapping
pointers." Without proof or `restrict`, GCC may refuse.

**Step 2 — build with opt-info.** `2> vec.txt` captures messages. The rejected wrong
reading is: "no message means no SIMD ever." Messages are for optimized vectorizations;
also confirm with `objdump`.

**Step 3 — inspect.** Packed ops on the vectorized path; scalar on the hazard path.

## Distinctions worth keeping straight

- **`restrict` promise vs actual overlap** — lying is undefined behavior territory.
- **Opt-info vs objdump** — compiler narrative versus bytes emitted.
- **`-O3 -march=native` vs `-O2`** — autovec often needs the higher drive + ISA.
- **Clear no-alias call sites vs hazardous signatures** — both teach.

## Check yourself

1. Why can aliasing legally prevent vectorization?
2. What flag asks GCC to report vectorized loops here?
3. What asm shapes suggest SIMD happened?
4. What does `restrict` mean as a promise?

## Key takeaways

- Autovectorization is real and inspectable on this toolchain.
- Aliasing uncertainty blocks transforms; `restrict` clears them when true.
- Opt-info plus objdump beat speculation.
- Correctness of the promise is on you.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
