# 77 — Seam by linking (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`. Confirm the print leaf's six calls appear.
2. Open `render.h` and list the six functions from memory, then check.
3. Skim `render_ncurses.c.example`. Match each `render_*` to an ncurses
   call (`initscr`, `clear`, `mvaddch`, `mvprintw`, `refresh`, `endwin`).
4. In your own words (scratch file is fine), fill the book-name table from
   the lesson using *your* RPG paths — not the print stand-in.
5. Answer: if tests need a fake renderer *and* you still want to run the
   real one in the same debugging session, why is `BACKEND=` not enough?

## Done when

- `./prog` prints `ok` after the six render lines.
- You can name the six `render.h` entry points without looking.
- You can state the link-seam limit in one sentence (one implementation
  per binary).

## Lookup

`render.h`, lesson table, your `ch32/Makefile` `BACKEND` block.
