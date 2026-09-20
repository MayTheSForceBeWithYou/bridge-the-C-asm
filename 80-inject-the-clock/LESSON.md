# 80 — Inject the clock
<!-- concepts: c.p4.di.clock, c.p4.di.ambient -->

`time(NULL)` anywhere in a call graph is ambient context: any function can
reach the wall clock without admitting it in a parameter list. You already
refused that pattern for randomness by threading `Rng *` through
`SpellContext`, and then seeded once with `time(NULL)` at
`~/dev/c/LearnCwithRPG-terminal/code/ch24/game.c:1122`. After this lesson you
can freeze time in a test the same way you freeze a renderer, and you can say
why one ambient call at a composition root is fine while the same call inside
logic is not.

## What this lesson asks of you

Read `clock.h` and `cooldown.c`. Run the test binary, which drives a spell
cooldown through a frozen clock and advances that clock instead of sleeping.
Then write `student_clock.c` yourself and build it against the same
`prog.c`. The mechanism is exercise 78's ops struct; only the domain changed.

`TASK.md` is the practice. This file is the teaching.

## Ambient time versus an injected clock

The seam is one function pointer standing where `time(NULL)` used to be:

```c
struct Clock {
    time_t (*now)(void);
};
```

Production fills `.now` with `clock_real_now`, a thin wrapper over
`time(NULL)`. A test fills it with `clock_frozen_now` and moves the frozen
value with `clock_freeze`. The code under test — `stamp_event` and
`cooldown_ready` — never names `time`. Every answer arrives through the
parameter, which means the caller decides what "now" means and the callee
cannot cheat.

That last clause is the part worth internalizing. A dependency you can reach
without declaring is a dependency you cannot substitute, and substitutability
is the whole game. `Rng *` and `struct Clock *` are the same move against two
different sources of nondeterminism.

## One real call, at one known place

Injecting the clock does not mean nothing ever calls `time(NULL)`. Something
must, or the program never learns the date. The rule is about *where*: one
call, at the composition root, where a reader looking for "how does this
program get the time" can find it in one grep.

Your seed site at `game.c:1122` is exactly that shape and needs no defense.
The pattern to refuse is the second and third call — the one inside combat
resolution, the one inside the save routine — each of which makes its
enclosing function untestable without the machine cooperating.

## Why freezing beats tolerating

The tempting alternative is to let real time flow and assert loosely: call
the code, read the clock, check the stamp is "about now," allow a second or
two of slack. Three things go wrong. A loaded CI agent blows through the
tolerance and the suite goes red for no reason. A breakpoint during debugging
does the same. And most importantly, a passing loose assertion does not prove
the code used your seam at all — code that ignored the `Clock *` and called
`time(NULL)` directly would sail through it.

A frozen clock has no tolerance window because it has no drift. Two calls
return the identical value, so `CHECK_EQ` is exact rather than approximate.
Better still, you can *move* time: freeze at one instant, assert a cooldown
has not expired, freeze twenty-five seconds later, assert it has. No sleep,
no flake, and a test that fails loudly if the code under test ever caches a
timestamp or reaches around the seam.

## Worked example

`prog.c` freezes the clock at `1700000000`, stamps twice, and checks both
stamps are exactly that. Then it starts a thirty-second cooldown ten seconds
in the past and checks it is not ready; advances the frozen clock to
`1700000025`; and checks it is:

```text
$ make && ./prog; echo exit=$?

6 checks, 0 failed
exit=0
```

The last check is the one doing the most work:

```c
CHECK(stamp_event(&real) != stamp_event(&frozen));
```

Both leaves are live in the same process at once — the real clock and the
frozen one — and `stamp_event` answers differently depending on which struct
it is handed. That is a property "assert the stamp is roughly now" could
never establish, and it is available only because the leaf is a value rather
than a link-time choice.

It is tempting to treat `cooldown_ready` as too trivial to inject: it is one
subtraction and a comparison. That is wrong, because triviality is not the
criterion — reachability is. The subtraction is fine; the `now()` it
subtracts *from* is the untestable part, and it would be equally untestable
inside a hundred-line function. Inject at the boundary where nondeterminism
enters, whatever the size of the code around it.

## Distinctions worth keeping straight

- **Ambient `time(NULL)` vs `const struct Clock *`.** A hidden dependency
  versus a named one.
- **Seed site vs call sites.** One real `time` at a composition root is
  correct; scattering `time` through logic is not.
- **Freezing vs sleeping.** Determinism in microseconds, versus wall-clock
  hope in seconds.
- **Advancing a frozen clock vs re-running the test.** Moving time is how you
  test elapsed-time behavior without elapsed time.

## Check yourself

1. Where does your RPG currently call `time(NULL)` for seeding, and why does
   that one call not need fixing?
2. What does a frozen clock return on the second stamp, and why does that
   let you use `CHECK_EQ` instead of a tolerance?
3. Give a concrete bug that "assert the stamp is roughly now" would miss.
4. Who fills `struct Clock` in production, and who fills it in a test?
5. How would you test a two-hour cooldown without waiting two hours?

## Key takeaways

- Time is a dependency like any other; put it in a parameter and inject it.
- One real `time(NULL)` at the composition root is correct; the second one is
  the smell.
- Frozen clocks make time assertions exact, and advancing them tests elapsed
  behavior in zero elapsed time.
- Loose "roughly now" assertions are flaky *and* fail to prove the seam was
  used.
- Same judgement as the renderer seam: name what you reach for.

## Lookup (not the lesson)

- `man 2 time`, `man 3 difftime`
- `~/dev/c/LearnCwithRPG-terminal/code/ch24/game.c:1122`, and `ch24/rng.h`
  for the `Rng *` precedent

Now open `TASK.md` and do the practice.
