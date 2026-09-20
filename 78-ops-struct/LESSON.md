# 78 — From link seam to ops struct
<!-- concepts: c.p4.di.opsstruct, c.p4.fnptr.call -->

A link seam picks one leaf per binary. A `struct` of function pointers picks
a leaf per *call* inside one process. After this lesson you can say when the
extra indirection is worth it — and when it is ceremony — and you can read a
`const struct RenderOps *` parameter as a runtime seam.

## What this lesson asks of you

Study `render_ops.h` and the print leaf in `render_ops.c`. Run `prog` and
watch calls go through the struct. Then fill `student_ops.c` yourself (it is
not linked by default). The judgement in this lesson matters more than the
typing: you already shipped the link seam; this shape exists for tests.

## Why the link seam stopped being enough

Exercise 77's seam substitutes `render_ncurses.c` for `render_sdl3.c` at
link time. That is the right tool for shipping one game binary. A unit test
that asserts on draw calls needs a different leaf **in the same process**
as the code under test — often in the same `main` as other tests. You cannot
link both the real renderer and a recording fake into one binary under the
six-global-function design without renaming symbols. A struct of pointers
makes the leaf a value you pass.

## The ops struct

```c
struct RenderOps {
    void (*clear)(void);
    void (*draw_tile)(int x, int y, int tile);
    void (*present)(void);
};
```

Three fields, three function pointers. A print leaf assigns them to
functions that `printf`. A later fake leaf assigns them to functions that
record. `draw_scene(const struct RenderOps *ops, ...)` never names a leaf
— it only calls through `ops`. That parameter *is* the seam.

## Worked example

`make` and `./prog` produce:

```text
clear
draw_tile 3 4 2
present
ok
```

`main` stacks a `struct RenderOps`, `render_print_ops` fills it, `draw_scene`
calls through it. No ncurses, no SDL — and the call sites in `draw_scene`
would look identical if `ops` pointed at a fake.

It is tempting to read this struct as "always better than a link seam."
That is wrong, because for a shipped game you still want one leaf, one
binary, and the simpler global `render_draw_tile` API. The struct buys you
**two implementations in the same process at the same time**, which is what
a test needs. Prefer the link seam for shipping; prefer the struct when a
test must substitute a leaf without forking a second binary's worth of
link graph.

## Distinctions worth keeping straight

- **Link seam vs ops struct.** One leaf per binary vs leaf-as-value.
- **Shipping vs testing.** Simpler API for the product; indirection for the
  assertion.
- **Function pointer field vs `BACKEND=`.** Runtime choice versus build-time
  choice.
- **`student_ops.c` vs `render_ops.c`.** Your rewrite versus the linked
  reference — same shape.

## Check yourself

1. What can an ops struct do in one process that `BACKEND=` cannot?
2. Why keep the link seam for the shipped game at all?
3. Who chooses the leaf when the parameter is `const struct RenderOps *`?
4. Name the three fields in this lesson's `RenderOps`.
5. Does `draw_scene` know whether it is printing or recording?

## Key takeaways

- Runtime seams pass behavior as data (a struct of pointers).
- That is the prerequisite for an in-process fake.
- Link seams stay correct and simpler for one shipped binary.
- The composition root moves from Make's `BACKEND=` into whoever fills
  and passes the struct — often `main` or a test's setup.
- Exercise 79 puts a recording leaf behind this same struct.

## Lookup (not the lesson)

- `render_ops.h` in this directory
- Lesson 15 — function pointers as values

Now open `TASK.md` and do the practice.
