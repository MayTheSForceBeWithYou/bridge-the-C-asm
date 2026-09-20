# 82 — Fake the syscall
<!-- concepts: c.p4.di.syscall, c.p5.asm.syscall -->

Exercises 26 and 40 taught you to issue `write` and `open` yourself. This
one wraps those entry points behind `struct SysOps` so a test can count
bytes without touching the kernel. Same injection shape as the clock and
the file reader — one level closer to the metal.

## What this lesson asks of you

Run the fake `write` test. Skim your earlier `26-asm-write-syscall` and
`40-asm-open-read` notes. Optionally fill `student_greet.c`.

## Ops at the syscall boundary

```c
struct SysOps {
    ssize_t (*write)(int fd, const void *buf, size_t n);
    int (*open)(const char *path, int flags, int mode);
};
```

Real leaf: thin wrappers over libc `write` / `open` (or your asm stubs).
Fake leaf: record counts, return success, never enter the kernel.
`greet` only talks to `ops->write`.

## Worked example

`./prog` drives `greet` through the fake, asserts one write of three bytes
(`hi\n`), exits 0. It is tempting to assert by redirecting stdout and
reading a pipe. That is wrong for *this* claim, because you would be
testing the test harness's pipe plumbing as much as `greet` — and you
still could not fake a failing `write` without more shell theatre.

## Distinctions worth keeping straight

- **Asm syscall labs vs injection labs.** One teaches the kernel ABI; the
  other teaches substituting it.
- **Fake write vs capturing stdout.** Deterministic counts versus process
  I/O setup.
- **Errno-rich failure paths.** A fake can return `-1`; a pipe redirect
  fights you.

## Check yourself

1. What does the fake `write` not do?
2. Why revisit exercises 26 and 40 here?
3. Who supplies `SysOps` in a unit test?
4. Name one failure mode a fake can simulate that stdout capture cannot
   easily.

## Key takeaways

- Syscalls are dependencies too.
- Fakes record or fail on demand; they skip the kernel.
- Track B taught the metal; Track H teaches the seam above it.

## Lookup (not the lesson)

- `man 2 write`, `man 2 open`
- Exercises 26, 40

Now open `TASK.md` and do the practice.
