# 56 — Same loop, three optimization stories

Optimization levels change instruction shape; cycle counts make that change visceral.
You already read `-O0` vs `-O2` assembly in Track A. Now you time the same `work()`
under `-O0`, `-O2`, and `-O3 -march=native`, and *tie* the speedup to what `objdump`
shows — not to vibes.

## What this lesson asks of you

Finish a simple dependent loop in `work()`, keep `rdtscp_now` working, capture timings
and assembly at three levels, and explain the cycle ratio from the instruction-level
differences.

## What changes across -O

At `-O0`, expect heavy stack traffic, redundant moves, and a loop that looks like your
C. At `-O2`, expect tighter register use, fewer loads/stores, maybe strength reduction.
At `-O3 -march=native`, expect further aggression — sometimes vectorization (exercise
57), sometimes just better scalar code.

A *dependent* loop (each iteration needs the previous result) limits parallel magic;
speedups still appear from removing memory sloshing. If the compiler deletes an unused
result, your timer becomes fiction — keep the result observable (`volatile`, print, or
escape hatch).

## Protocol

Build cleanly between modes so objects do not mix:

```
make clean && make O=0 && ./prog | tee o0.txt
make clean && make O=2 && ./prog | tee o2.txt
make clean && make CFLAGS='-ggdb -Wall -Wextra -O3 -march=native' && ./prog | tee o3.txt
```

Save `prog.O0.s` / `prog.O2.s` (and O3 if you like) for diffs.

## Protocol (do not mix objects)

```text
make clean && make O=0 && ./prog | tee o0.txt
make clean && make O=2 && ./prog | tee o2.txt
make clean && make CFLAGS='-ggdb -Wall -Wextra -O3 -march=native' && ./prog | tee o3.txt
```

Save asm dumps (`prog.O0.s`, `prog.O2.s`, …) for diffs.

## What usually changes across `-O`

| Level | Typical shape |
| ----- | ------------- |
| `-O0` | stack traffic, redundant moves, C-shaped loop |
| `-O2` | tighter regs, fewer spills — often most of the win for dependent scalar loops |
| `-O3 -march=native` | further aggression; maybe SIMD (57) — not guaranteed to crush `-O2` here |

**Rejected wrong reading:** "`-O3` must always crush `-O2` on every loop."

## Tie cycles to instructions

Point at concrete removed spills / tighter add chains. **Rejected wrong reading:** "the
cycle gap is unexplained magic."

## Keep work observable

If the compiler deletes an unused result, your timer is fiction — `volatile`, print, or
escape the result.

## Worked example

**The situation.** `-O0` takes tens of millions of cycles; `-O2` is several times
faster; asm at `-O0` shows stack stores your `-O2` dump killed.

**Step 1 — time all three.** Record medians or all three prints per binary. The
rejected wrong reading is: "`-O3` must always crush `-O2` on every loop." Dependent
scalar loops often see most of the win by `-O2`.

**Step 2 — diff asm.** Point at concrete instructions: removed spills, tighter add
chain. The rejected wrong reading is: "the cycle gap is unexplained magic." Your Track A
eyes should find *some* structural reason even if incomplete.

**Step 3 — write the link.** One paragraph: shape → why fewer cycles.

## Worked explanation paragraph (what to write)

"At `-O0`, `work` spills/reloads each iteration (point at `mov` to stack in
`prog.O0.s`). At `-O2`, the dependent add chain stays in registers (point at the tight
loop). Cycle ratio ≈ ____, explained by removing memory sloshing, not by magic."

If `-O3 -march=native` barely beats `-O2`, say so — dependent scalar loops often win most
by `-O2`.

## Distinctions worth keeping straight

- **Opt level vs march=native** — algorithm/IR opts versus enabling ISA features.
- **Dependent vs independent loops** — different headroom for ILP/SIMD.
- **Dead-code elimination vs "faster math"** — ensure you still measure real work.
- **One run vs ensembles** — still note variance (lesson 55).

## Deeper worked navigation (opt levels timed)

- clean between O levels.
- Tie cycle ratio to asm diffs.
- Dependent loops often win most by O2.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| o0.txt/o2.txt | cycle ratios |
| asm dumps | spills removed |
| notes | shape→cycles paragraph |

### Ordered navigation moves

1. Time O0/O2/O3native.
2. Diff asm.
3. Write causal paragraph.

### Rejected wrong readings (keep beside the artifact)

- O3 always crushes O2.
- cycle gap is unexplained magic.
- mix objects across O=.

### Tool-line decoding reminders

- make clean discipline
- spill vs reg chain
- DCE hazards

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (56)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - mixed O= objects
- O3 must crush O2
- no asm citation for cycle gap

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

1. Why rebuild clean between `O=` values?
2. What kind of asm differences typically explain `-O0` vs `-O2` on a tiny loop?
3. Why might `-O3 -march=native` barely beat `-O2` here?
4. How do you keep the compiler from deleting `work()`?

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

- Timings without asm diffs are incomplete stories.
- Most scalar cleanup often appears by `-O2`.
- `rdtscp` makes opt-level education quantitative.
- Always preserve observable side effects of the timed work.

## Field-decoding recap for exercise 56

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 55

Now open `TASK.md` and do the practice.
