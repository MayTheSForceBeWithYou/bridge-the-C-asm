# 82 — Fake the syscall
<!-- concepts: c.p4.di.syscall, c.p5.asm.syscall -->

Exercises 26 and 40 had you issue `write` and `open` yourself, with the
arguments in the right registers and `syscall` at the bottom. This lesson
wraps those same entry points behind a struct so a test can drive them
without a kernel — and, more usefully, can make them fail. After this lesson
you can name the failure states a fake produces on demand that a real file
descriptor will only hand you by accident.

## What this lesson asks of you

Read `sys_ops.h` and `greet.c`. Run the test, which exercises a full write, a
refused write, and a short write against the same code. Then write
`student_greet.c` and build it against the unmodified `prog.c`. Two of the
seven checks are there specifically to fail a plausible implementation; find
out which before you read the answer in the worked example.

`TASK.md` is the practice. This file is the teaching.

## Ops at the syscall boundary

```c
struct SysOps {
    ssize_t (*write)(int fd, const void *buf, size_t n);
    int (*open)(const char *path, int flags, int mode);
};
```

These signatures are the kernel's, not libc's conveniences — `ssize_t` back,
a byte count in, no buffering, no `FILE *`. The real leaves are one-line
wrappers over libc's `write` and `open`, which are themselves thin wrappers
over the instruction you wrote by hand in Track B. The fake leaves count and
return, and never enter the kernel at all.

This is the third injection in a row with the same shape — renderer, clock,
file reader, now syscalls — and the repetition is the point. The technique
does not change as you descend; only what is on the far side of the seam
does.

## Short writes: the bug you cannot see by running the program

`write(2)` is permitted to transfer fewer bytes than you asked for and report
that smaller count. On a regular file it essentially never does. On a pipe, a
socket, or a terminal under pressure, it does. Code that calls `write` and
ignores the return value is therefore wrong in the specific, expensive way
that only shows up on someone else's machine, under load, months later.

You cannot test that against a real descriptor without contriving a full pipe
buffer and a reader that stops reading — and even then you are timing-
dependent. Against a fake it is one line:

```c
sys_fake_short_next();   /* the next write reports one byte fewer */
```

The same argument covers outright failure. `sys_fake_fail_next()` makes the
next write return `-1`, the state a real descriptor reaches by being closed
under you or by a broken pipe. Reproducing that for a test is theatre;
declaring it is a function call.

This is the strongest argument in Track H for fakes over capturing real
output. Redirecting stdout into a pipe and reading it back tests the happy
path perfectly well. It cannot make the kernel say no.

## Worked example

`greet` writes `hi\n` through `ops->write` and returns 0 only if every byte
was reported written:

```text
$ make && ./prog; echo exit=$?

7 checks, 0 failed
exit=0
```

Seven checks across three scenarios: a clean write (returns 0, one call,
three bytes), a refused write (returns -1, zero bytes recorded), and a short
write (returns -1, two bytes recorded).

Now the implementation a competent junior writes:

```c
ops->write(fd, msg, strlen(msg));
return 0;
```

It passes the first three checks — the write happens, the count is right, the
bytes are right — and fails exactly two: the refused case and the short case
both expect `-1` and get `0`. That is the exercise working as designed. The
happy-path assertions could never have caught it, because on the happy path
that code is correct.

It is tempting to assert only `sys_fake_write_count() == 1` and call the
boundary covered. That is wrong, because a call count constrains almost
nothing: it is satisfied by code that writes the wrong bytes, to the wrong
descriptor, and ignores a failure. Count the call if you like, but the claim
lives in the arguments and the return value.

## Distinctions worth keeping straight

- **Track B's syscall labs vs this one.** One teaches the kernel ABI; the
  other teaches substituting it.
- **Faking the syscall vs capturing stdout.** Deterministic counts and
  injectable failure, versus pipe plumbing that only does success.
- **`-1` vs a short count.** Two different failures, two different branches;
  a fake gives you both on demand.
- **Asserting a call happened vs asserting what it returned.** Counts are
  weak; return-value handling is where the bug lives.

## Check yourself

1. What does the fake `write` deliberately not do?
2. Name two states a real `write` reaches that you cannot arrange on purpose
   in a test.
3. Which of the seven checks fail for code that ignores `write`'s return
   value, and why do the others pass?
4. Why is `ssize_t` the return type rather than `int` or `size_t`?
5. When is capturing real stdout the better test?

## Key takeaways

- Syscalls are dependencies; the injection shape is the same one exercise 78
  introduced.
- A fake's real value is the failure states it produces on demand.
- Short writes are correct kernel behavior and a permanent source of bugs;
  test for them deliberately.
- Happy-path assertions cannot catch ignored return values — that is what the
  failure scenarios are for.
- Track B taught the metal; Track H teaches the seam above it.

## Lookup (not the lesson)

- `man 2 write` — in particular the return value and partial-write paragraphs
- `man 2 open`, `man 3 errno`
- Exercises 26 and 40 — the same entry points, by hand

Now open `TASK.md` and do the practice.
