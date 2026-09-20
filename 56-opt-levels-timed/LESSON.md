# 56 — Same loop, three optimization stories
<!-- concepts: c.p5.opt.levels -->

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

## Distinctions worth keeping straight

- **Opt level vs march=native** — algorithm/IR opts versus enabling ISA features.
- **Dependent vs independent loops** — different headroom for ILP/SIMD.
- **Dead-code elimination vs "faster math"** — ensure you still measure real work.
- **One run vs ensembles** — still note variance (lesson 55).

## Check yourself

1. Why rebuild clean between `O=` values?
2. What kind of asm differences typically explain `-O0` vs `-O2` on a tiny loop?
3. Why might `-O3 -march=native` barely beat `-O2` here?
4. How do you keep the compiler from deleting `work()`?

## Key takeaways

- Timings without asm diffs are incomplete stories.
- Most scalar cleanup often appears by `-O2`.
- `rdtscp` makes opt-level education quantitative.
- Always preserve observable side effects of the timed work.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, exercise 55

Now open `TASK.md` and do the practice.
