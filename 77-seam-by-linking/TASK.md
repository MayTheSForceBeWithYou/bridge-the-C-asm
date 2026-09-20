# 77 — Seam by linking (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`. Confirm the print leaf's six calls appear in
   source order, then `ok`.
2. Open `render.h` and list the six functions from memory, then check.
3. Skim `render_ncurses.c.example`. Match each `render_*` to its ncurses
   call (`initscr`, `clear`, `mvaddch`, `mvprintw`, `refresh`, `endwin`).
4. Write a second leaf — `render_null.c`, six functions that do nothing but
   return `true` from `render_init`. Link it instead of the print leaf:

   ```bash
   make clean && make SRCS="prog.c render_null.c"
   ```

   `./prog` should print only `ok`. You have now done by hand what
   `BACKEND=` does.
5. Open `~/dev/c/LearnCwithRPG-graphics/code/ch32/render.h` and count the
   declarations. Pick three that `render_ncurses.c` could not implement and
   say what a character grid lacks in each case.
6. Fill the lesson's book-name table using *your* RPG paths, not the print
   stand-in. A scratch file outside this repo is fine.

## Done when

- `./prog` prints six render lines then `ok`, and your `render_null.c` build
  prints only `ok`.
- You can name the six `render.h` entry points without looking.
- You can state the link-seam limit in one sentence, and name the one
  backend that the wider interface excluded.

## Lookup

`render.h`, the lesson's table, `ch32/Makefile`'s `BACKEND` block.
