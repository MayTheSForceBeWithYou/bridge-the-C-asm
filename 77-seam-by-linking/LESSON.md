# 77 — The seam you already built, named
<!-- concepts: c.p4.di.abstraction, c.p4.di.linkseam -->

You already shipped a renderer interface with interchangeable backends and a
Makefile knob that picks one at build time. This lesson teaches no new
mechanism. It gives the Dependency Injection book names to code you wrote, so
the next two exercises can say "runtime seam" and "fake" without inventing
vocabulary — and it looks closely at the one place your seam did not hold,
because that is where the interesting constraint lives.

## What this lesson asks of you

Read the two files copied into this directory from
`~/dev/c/LearnCwithRPG-terminal/code/ch24/`: `render.h` and
`render_ncurses.c.example`. Map each piece to the table below. Run the tiny
print backend so you see one leaf linked into `prog`. Then open `TASK.md`. Do
not "improve" the interface — name it.

Two chapter directories get referenced throughout Track H, and they are in
**different** sibling repositories — worth fixing in your head now, because
the lessons use the short forms after this:

| Short form | Full path |
| ---------- | --------- |
| `ch24/…` | `~/dev/c/LearnCwithRPG-terminal/code/ch24/` |
| `ch32/…` | `~/dev/c/LearnCwithRPG-graphics/code/ch32/` |

## The abstraction you already wrote

The terminal game's `render.h` is twenty-one lines. It declares six functions
and a tile enum:

- `render_init`
- `render_shutdown`
- `render_clear`
- `render_draw_tile`
- `render_draw_text`
- `render_present`

Nothing in game logic needs to know what is behind those six.
`render_ncurses.c` is one leaf: `initscr`, `clear`, `mvaddch`, `mvprintw`,
`refresh`, `endwin`. At link time exactly one translation unit defining those
six symbols goes into the binary, and the rest of the program cannot tell
which.

## Build-time composition, in your own words

From `~/dev/c/LearnCwithRPG-graphics/code/ch32/Makefile`, verbatim — the best
argument for the technique in your whole corpus:

> Which renderer to build against. Both implement render.h; nothing else
> in the program knows or can tell.
>
> Keeping the old backend BUILDING is not sentiment. A port that breaks
> the thing it is replacing has proved nothing about the seam, and there
> is no way back when the new one misbehaves.

`BACKEND ?= sdl3`, with `make BACKEND=gl` selecting the other, is a
composition root expressed as a Make variable. Change the variable, relink,
get a different leaf. That opacity *is* the seam.

## Where the seam did not hold, and why that is the real lesson

It is worth being precise about which backends were ever interchangeable,
because the honest version teaches more than the tidy one.

The terminal track's `render.h` is the twenty-one-line file above, with an
ncurses leaf. The graphics track's `ch32/render.h` is a *different* header:
199 lines, twenty functions, with textures, sprite blits, tinting, clipping,
screen shake, and shaders. The `BACKEND=` knob chooses between `render_sdl3.c`
and `render_gl.c` — both leaves of that second, wider interface. The ncurses
backend is not one of the choices. Your own Makefile says why:

> The terminal backend is gone: see Chapter 5 — `render_draw_sprite` is not a
> thing a character grid can implement, and a backend that cannot honour the
> interface is worse than no backend.

So there were two generations of the abstraction, not one abstraction with
three leaves. That distinction is the constraint every interface lives under:
**an abstraction stays useful only while every leaf you care about can
honour it.** Widening `render.h` to express sprites bought real capability and
cost the character-grid leaf, and no amount of dependency-injection discipline
could have preserved both. Deciding that the sprite API was worth losing
ncurses was a design judgement, made once, written down in a comment.

Notice what the seam still paid for even so. The port from SDL_Renderer to
OpenGL — a total replacement of the drawing implementation — happened behind
an unchanged header, with the old backend still building the whole time. That
is the win, and it is a large one. The seam bought substitution *within* a
generation of the interface; it did not and could not freeze the interface
forever.

## Names from the book, mapped to your C

Seemann & van Deursen's *Dependency Injection* uses C# wording. Same shapes,
your files:

| Your C | Book name |
| ------ | --------- |
| `render.h` (either generation) | the abstraction |
| `render_ncurses.c`, or `render_sdl3.c` / `render_gl.c` | leaf implementations |
| `main()` choosing paths and calling `render_init` | the composition root |
| `BACKEND=` in `ch32/Makefile` | build-time composition |
| `time(NULL)` at exactly one call site (`ch24/game.c:1122`) | avoiding ambient context |
| `Rng *rng` threaded through `SpellContext` | injecting nondeterminism |

Ambient context means "any code may reach the wall clock or a global without
saying so in its signature." You already refused that for the RNG by threading
`Rng *`. The seed at `ch24/game.c:1122` is the one remaining ambient call —
named here so exercise 80 has somewhere to point.

## Worked example

This directory links `prog.c` against `render_print.c`, a leaf that prints
instead of drawing. Same six symbols as the ncurses leaf. `make` and `./prog`:

```text
render_init
render_clear
render_draw_tile 1 2 2
render_draw_text 0 0 ok
render_present
render_shutdown
ok
```

The `1 2 2` line is `render_draw_tile(1, 2, TILE_PLAYER)`, and the trailing
`2` is `TILE_PLAYER`'s value in the enum — third constant, zero-based. Swap
`render_print.c` out of `SRCS` for another file defining the same six symbols
and you have performed by hand the substitution `BACKEND=` performs for you.

It is tempting to read "seam" as "I wrote an interface, so I can unit-test the
renderer boundary." That is wrong, because a link-time seam gives you
substitution of **one implementation per binary**. The test binary and the
game binary are different links. You cannot, inside one process, run the real
backend for a smoke test and a recording fake for an assertion at the same
time. That limit is why exercise 78 exists.

## Distinctions worth keeping straight

- **Abstraction vs leaf.** The header names the contract; `.c` files fulfill it.
- **Link-time seam vs runtime seam.** One leaf per binary, versus a struct of
  function pointers you can swap inside one process (next exercise).
- **Composition root vs ambient context.** Who chooses the leaf, and who is
  forbidden from reaching for globals.
- **Widening an interface vs substituting behind it.** The second is free; the
  first can cost you a leaf, as it cost you ncurses.
- **Keeping the old backend building vs deleting it early.** Your Makefile
  comment already settled this; believe it.

## Check yourself

1. Name all six functions in the twenty-one-line `render.h`.
2. Does `BACKEND=` select a header or a translation unit?
3. Why does a link seam alone not give you an in-process test double?
4. Which two backends were actually interchangeable, and which one was not?
   What made the difference?
5. Where does your RPG seed the RNG, and why does that site matter for DI?

## Key takeaways

- You already built an abstraction and several leaves; this lesson only names
  them.
- Link-time composition substitutes one implementation per binary — enough to
  ship, not enough to assert on draw calls in the same process.
- An abstraction survives only as long as every leaf can honour it; the
  sprite API bought capability and cost the terminal backend.
- The seam still earned its keep: SDL_Renderer to OpenGL, behind an unchanged
  header, with a working fallback the whole way.
- Ambient `time(NULL)` and a threaded `Rng *` are opposite moves on one axis.

## Lookup (not the lesson)

- This directory's `render.h` and `render_ncurses.c.example` (copies of
  `~/dev/c/LearnCwithRPG-terminal/code/ch24/`)
- `~/dev/c/LearnCwithRPG-graphics/code/ch32/Makefile` — the `BACKEND` block
- Seemann & van Deursen, *Dependency Injection* — composition root, ambient
  context (concepts, not C# APIs)

Now open `TASK.md` and do the practice.
