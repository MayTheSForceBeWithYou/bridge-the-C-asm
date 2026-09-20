# 80 — Inject the clock
<!-- concepts: c.p4.di.clock, c.p4.di.ambient -->

`time(NULL)` anywhere in the call graph is ambient context: every function
can reach the wall clock without admitting it in a parameter list. Your RPG
already refused that pattern for RNG (`Rng *` on `SpellContext`) but still
seeds once with `time(NULL)` at `ch24/game.c:1122`. After this lesson you
can freeze time in a test the same way you freeze a renderer — pass a
`struct Clock`.

## What this lesson asks of you

Read `clock.h`. Run the test binary that stamps through a frozen clock.
Optionally fill `student_clock.c`. The mechanism is the same ops-struct
move as exercise 78; the domain is time.

## Ambient time vs an injected clock

```c
struct Clock {
    time_t (*now)(void);
};
```

Production `main` sets `.now = clock_real_now` (a thin wrapper over
`time(NULL)`). Tests call `clock_freeze(t)` and set `.now =
clock_frozen_now`. `stamp_event(const struct Clock *clock)` never names
`time`. That is the whole design.

## Worked example

`./prog` checks that two stamps under a frozen clock return the same
`1700000000` and exits 0. It is tempting to call `time(NULL)` inside the
test and "assert it is roughly now." That is wrong, because a slow CI
agent, a leap second, or a debug pause makes "roughly" flaky — and you
still have not proved the code used *your* seam. Freeze the value; assert
equality.

## Distinctions worth keeping straight

- **Ambient `time(NULL)` vs `Clock *`.** Hidden dependency versus named one.
- **Seed site vs call sites.** One real `time` at composition root is fine;
  scattering `time` inside logic is not.
- **Frozen clock vs sleeping in tests.** Determinism versus wall-clock hope.

## Check yourself

1. Where does your RPG currently call `time(NULL)` for seeding?
2. What does a frozen clock return on the second stamp?
3. Why is "assert time is near now" a weak test?
4. Who fills `struct Clock` in production versus in a test?

## Key takeaways

- Time is just another dependency; inject it.
- Freeze in tests; use `time(NULL)` only at the composition root.
- Same judgement as the renderer seam — name what you reach for.

## Lookup (not the lesson)

- `man 2 time`
- `ch24/game.c:1122`, `ch24/rng.h`

Now open `TASK.md` and do the practice.
