# 78 — Ops struct (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` and run `./prog`. Match each printed line to a field in
   `struct RenderOps`.
2. Read `draw_scene`. Confirm it never names `print_*` — only `ops->`.
3. Fill `student_ops.c` (TODOs). Optionally point `SRCS` at it instead of
   `render_ops.c` and rebuild to prove your version.
4. In one sentence: when would you refuse this struct and keep the link
   seam?

## Done when

- `./prog` prints clear / draw_tile / present / ok with the linked
  reference.
- `student_ops.c` still has your work (or TODOs if you have not started).
- You can defend "link for ship, struct for test" without reading the
  lesson again.

## Lookup

`render_ops.h`, lesson 15.
