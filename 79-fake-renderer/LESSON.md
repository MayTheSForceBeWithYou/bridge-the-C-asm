# 79 — Fake renderer, assert on decisions
<!-- concepts: c.p4.di.fake, c.p3.test.check -->

Your RPG has three real renderers and zero test doubles. The seam was built
for shipping backends, then the test binary avoided the boundary by linking
no renderer at all. After this lesson you can record `draw_tile` calls in a
fake, assert on coordinates, and name the two ways a fake usually goes wrong.

## What this lesson asks of you

Read `player.c` (`try_move`) and the reference fake in `render_fake_ref.c`.
Run `prog` — it must exit 0. Then implement the same recording leaf in
`render_fake.c` (TODOs) and optionally relink. The load-bearing assertion is
not "draw was called"; it is "walking into a wall draws the player at the
**old** position."

## Why this was missing in your corpus

`ch32/Makefile`'s `TEST_SOURCES` lists pure modules and **no** renderer.
That is a real, common, defensible choice: unit-test combat math without a
window. It is also exactly why the draw boundary has never been tested. The
link seam gave you substitution for shipping; the tests stepped around the
seam instead of driving it with a double.

## A fake records; it does not decide

`render_fake_ref.c` implements `struct RenderOps`. On `draw_tile` it appends
`(x, y, tile)` into a fixed array and bumps a count. `clear` / `present` are
no-ops for this claim. There is no pathfinding, no occlusion, no "smart"
redraw — if the fake grew logic worth testing, it would be a second
implementation, not a double.

Because `RenderOps` function pointers take no userdata, the fake keeps a
file-scope `g_fake` pointer set in `render_fake_init`. That is a deliberate
narrowing for the exercise, not a pattern to copy into large systems (those
pass a `void *ctx` or embed ops at the front of a wider struct).

## Worked example

`try_move` with `wall_ahead == 1` leaves `p.x` / `p.y` unchanged, then
draws `TILE_PLAYER` at those coordinates through `ops`. The test does:

```c
try_move(&p, 1, 0, 1, &fake.ops);
CHECK_EQ(p.x, 5);
CHECK_EQ(fake.draws[0].x, 5);
CHECK_EQ(fake.draws[0].y, 5);
```

On this machine `./prog` prints `9 checks, 0 failed` and exits 0.

It is tempting to assert only `CHECK_EQ(fake.draw_count, 1)` and call it
done. That is wrong, because "draw_tile was called once" barely constrains
behavior — a bug that draws the player on the far side of the wall still
passes a call-count check. The coordinates are the actual claim.

## Distinctions worth keeping straight

- **Fake that records vs fake that reimplements.** If it decides, it is not
  a double.
- **Asserting a call happened vs asserting what was decided.** Counts are
  weak; arguments are the claim.
- **Avoiding the renderer in tests vs substituting a fake.** Your RPG did
  the first; this exercise does the second.
- **`render_fake.c` (TODO) vs `render_fake_ref.c` (linked).** Your work
  versus the reference the harness ships with.

## Check yourself

1. What behavioral claim does the wall-bump test make?
2. Why is `draw_count == 1` nearly worthless alone?
3. Why did `TEST_SOURCES` without a renderer leave this gap?
4. What goes wrong if the fake starts computing visibility?
5. Why does this fake need `g_fake` at all?

## Key takeaways

- A recording fake turns draw calls into data a `CHECK_EQ` can see.
- Assert on the decision (old coordinates), not merely on the call.
- Link seams ship backends; ops structs plus fakes test boundaries.
- Skipping the renderer in unit tests is defensible and leaves a hole.
- Do not let the fake grow a second game.

## Lookup (not the lesson)

- `../test/check.h`
- Lesson 78 — ops struct
- Your `ch32/Makefile` `TEST_SOURCES` line

Now open `TASK.md` and do the practice.
