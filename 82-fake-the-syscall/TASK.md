# 82 — Fake the syscall (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` && `./prog` — exit 0.
2. Glance at `26-asm-write-syscall` / `40-asm-open-read` TASK files.
3. Optional: implement `student_greet.c` and relink.

## Done when

- Fake write count and byte total match `hi\n`.
- You can say what the fake replaces (the kernel entry), not just "I/O."

## Lookup

`man 2 write`, lessons 78–81.
