# 78 — Ops struct (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make && ./prog; echo $?`. Match each printed line to a field in
   `struct RenderOps`, then note that ten checks ran against a *second* leaf
   you did not see print anything.
2. Read `draw_scene` in `render_ops.c`. Confirm it never names `print_*` —
   only `ops->`. That is the property the next exercise depends on.
3. Fill `student_ops.c` and build it in place of the reference:

   ```bash
   make clean && make SRCS="prog.c student_ops.c"
   ```

   `./prog` should print the same four lines.
4. Read the counting leaf at the top of `prog.c`. Add a fourth check of your
   own: call `draw_scene` twice through it and assert what `ncalls` becomes.
   Decide first whether the harness should reset between scenes — exercise 79
   makes that decision explicit with `render_fake_reset`.
5. Break `draw_scene` on purpose: drop the `ops->clear()` call. Predict which
   checks fail before you rebuild, then rebuild.
6. In one sentence: when would you refuse this struct and keep the link seam?

## Done when

- Both the reference build and your `student_ops.c` build print the trace,
  then `10 checks, 0 failed`, then `ok`, and exit 0.
- Your prediction in step 5 matched which checks went red.
- You can defend "link for ship, struct for test" without rereading the
  lesson.

## Lookup

`render_ops.h`, lesson 15 (function pointers as values).
