# 77 — The seam you already built, named
<!-- concepts: c.p4.di.abstraction, c.p4.di.linkseam -->

You already shipped a renderer interface with three backends and a Makefile
knob that picks one at build time. This lesson teaches no new mechanism. It
gives the Dependency Injection book names to *your* code so the next two
exercises can say "runtime seam" and "fake" without inventing vocabulary
from scratch.

## What this lesson asks of you

Read the copies of `render.h` and `render_ncurses.c.example` in this
directory, and the Makefile quote below. Map each piece to the table in
this lesson. Run the tiny print backend so you see one leaf linked into
`prog`. Then open `TASK.md`. Do not "improve" the interface — name it.

## The abstraction you already wrote

`render.h` is twenty-one lines. It declares six functions and a tile enum.
Nothing in game logic needs to know whether those six are backed by
ncurses, SDL3, or OpenGL:

- `render_init`
- `render_shutdown`
- `render_clear`
- `render_draw_tile`
- `render_draw_text`
- `render_present`

`render_ncurses.c` (copied here as `render_ncurses.c.example`) is one leaf
implementation: `initscr`, `mvaddch`, `refresh`. Your graphics track adds
`render_sdl3.c` and `render_gl.c` against the same header. At link time
exactly one `.c` that defines those six symbols goes into the binary.

## Build-time composition, in your own words

Quote this verbatim from your `ch32/Makefile` — it is the best argument in
the corpus:

> Which renderer to build against. Both implement render.h; nothing else
> in the program knows or can tell.
>
> Keeping the old backend BUILDING is not sentiment. A port that breaks
> the thing it is replacing has proved nothing about the seam, and there
> is no way back when the new one misbehaves.

`BACKEND ?= sdl3` (or `gl`, or earlier `ncurses`) is the composition root
expressed as a Make variable. Change the variable, relink, get a different
leaf. The rest of the program cannot tell — that opacity *is* the seam.

## Names from the book, mapped to your C

Seemann & van Deursen's *Dependency Injection* uses C# wording. Same
shapes, your files:

| Your C | Book name |
| ------ | --------- |
| `render.h` | the abstraction |
| `render_ncurses.c` / `render_sdl3.c` / `render_gl.c` | leaf implementations |
| `main()` choosing paths / calling init | the composition root |
| `BACKEND=` in the Makefile | build-time composition |
| `time(NULL)` at exactly one call site (`ch24/game.c:1122`) | avoiding ambient context |
| `Rng *rng` passed through `SpellContext` | constructor-ish injection of nondeterminism |

Ambient context means "any code can call `time(NULL)` or read a global."
You already refused that for the RNG by threading `Rng *` through
`SpellContext`. The clock at line 1122 is the remaining single seed site —
named here so exercise 80 has somewhere to point.

## Worked example

This directory links `prog.c` against `render_print.c`, a leaf that prints
instead of drawing. Same six symbols. `make` and `./prog` show:

```text
render_init
render_clear
render_draw_tile 1 2 2
render_draw_text 0 0 ok
render_present
render_shutdown
ok
```

Swap `render_print.c` out of the Makefile's `SRCS` for another file that
defines the same six functions and you have performed the link-time
substitution your RPG Makefile already does with `BACKEND=`.

It is tempting to read "seam" as "I wrote an interface, so I can unit-test
the renderer boundary." That is wrong, because a link-time seam gives you
substitution of **one implementation per binary**. The test binary and the
game binary are different links. You cannot, in one process, run the real
ncurses leaf for a smoke test and a recording fake for an assertion side
by side. That limit is why exercise 78 exists.

## Distinctions worth keeping straight

- **Abstraction vs leaf.** Header names the contract; `.c` files fulfill it.
- **Link-time seam vs runtime seam.** One leaf per binary versus a struct of
  function pointers you can swap inside one process (next exercise).
- **Composition root vs ambient context.** Who chooses the leaf, and who is
  forbidden from reaching for globals.
- **Keeping the old backend building vs deleting it early.** Your Makefile
  comment already settled this; believe it.

## Check yourself

1. Name all six functions in `render.h`.
2. What does `BACKEND=` select — a header, or a translation unit?
3. Why does a link seam alone not give you an in-process test double?
4. Where does your RPG seed the RNG, and why does that matter for DI?
5. What does "keeping the old backend BUILDING" protect you from?

## Key takeaways

- You already built an abstraction and three leaves; this lesson only names
  them.
- Link-time composition substitutes one implementation per binary.
- That is enough to ship; it is not enough to assert on draw calls in the
  same process as the code under test.
- Ambient `time(NULL)` and a threaded `Rng *` are opposite moves on the
  same axis.
- The next exercise turns the six functions into a `struct` of pointers.

## Lookup (not the lesson)

- Your copies: `render.h`, `render_ncurses.c.example`
- Seemann & van Deursen, *Dependency Injection* — composition root,
  ambient context (concepts, not C# APIs)

Now open `TASK.md` and do the practice.
