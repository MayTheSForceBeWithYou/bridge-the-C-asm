# 57 — Autovectorization and the aliasing hazard

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

## Mapping `make` to vectorization evidence

| You type | Check afterward |
| -------- | --------------- |
| build with `-O3 -march=native -fopt-info-vec-optimized` | `vec.txt` (stderr redirect) |
| read `vec.txt` | which loops report vectorized |
| `objdump -d` | packed ops (`paddd` / `vaddps` / similar) on hot paths |

## Why aliasing blocks vectorization

If `dst` and `src` might overlap, wide loads/stores can change semantics. Compilers stay
conservative until proof or `restrict`.

**Rejected wrong reading:** renaming variables is enough for the compiler to vectorize
overlapping pointers.

## Opt-info vs objdump

| Evidence | Role |
| -------- | ---- |
| `-fopt-info-vec-optimized` | compiler narrative of which loops vectorized |
| `objdump` packed arithmetic | bytes actually emitted |

**Rejected wrong reading:** "no message means no SIMD ever" — also confirm with objdump.

## `restrict` is a promise

Lying about non-aliasing is undefined-behavior territory. Disjoint arrays + honest
`restrict` is the contract.

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

## Three functions, three contracts

| Function | Contract | Expect |
| -------- | -------- | ------ |
| `add_arrays` | clearly disjoint | vectorize |
| `add_hazard` | may alias | stay scalar |
| `add_restrict` | `restrict` promise | vectorize when true |

Build with opt-info redirected to `vec.txt`. **Navigation:** search `vec.txt` for your
function names; confirm packed ops in `objdump` for the vectorized ones.

## Distinctions worth keeping straight

- **`restrict` promise vs actual overlap** — lying is undefined behavior territory.
- **Opt-info vs objdump** — compiler narrative versus bytes emitted.
- **`-O3 -march=native` vs `-O2`** — autovec often needs the higher drive + ISA.
- **Clear no-alias call sites vs hazardous signatures** — both teach.

## Deeper worked navigation (autovec restrict)

- Aliasing blocks vectorization.
- restrict is a promise.
- opt-info + objdump together.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| vec.txt | which loops vectorized |
| objdump | packed ops |
| hazard fn | stays scalar |

### Ordered navigation moves

1. Implement three contracts.
2. Build with opt-info.
3. Confirm asm.

### Rejected wrong readings (keep beside the artifact)

- renaming proves non-aliasing.
- no message ⇒ no SIMD ever.
- lie with restrict safely.

### Tool-line decoding reminders

- opt-info lines
- packed mnemonic examples
- restrict semantics

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (57)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - rename implies non-alias
- opt-info silence means no SIMD ever
- lied with restrict

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

1. Why can aliasing legally prevent vectorization?
2. What flag asks GCC to report vectorized loops here?
3. What asm shapes suggest SIMD happened?
4. What does `restrict` mean as a promise?

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

- Autovectorization is real and inspectable on this toolchain.
- Aliasing uncertainty blocks transforms; `restrict` clears them when true.
- Opt-info plus objdump beat speculation.
- Correctness of the promise is on you.

## Field-decoding recap for exercise 57

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
