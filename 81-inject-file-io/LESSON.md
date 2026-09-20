# 81 — Inject file I/O
<!-- concepts: c.p4.di.fileops, c.p4.di.fake -->

A parser that calls `fopen` itself can only be tested with real files, which
means every parse test is also a test of paths, permissions, and whatever the
working directory happened to be. Lift the read behind a one-function struct
and the test hands the parser a string. After this lesson you can separate
"how bytes arrive" from "what the bytes mean," and you can say what the two
leaves must agree about for the substitution to be honest.

## What this lesson asks of you

Read `file_ops.h` and `parse.c`. Run the test, which feeds a parser three
different inputs — including a failed read — without touching the
filesystem. Then write `student_parse.c` against the same header and build it
in place of the reference. Finally compare the shape with
`~/dev/c/LearnCwithRPG-terminal/code/ch24/config.c`, which still opens its
path directly.

`TASK.md` is the practice. This file is the teaching.

## Two jobs wearing one coat

`config_load` in your RPG does two separable things: it gets bytes from
somewhere, and it interprets them. Tangled together, the interpretation can
only be exercised by arranging the world so the acquisition succeeds — create
a file, write known text, remember to delete it, hope the test runner's
working directory is what you assumed.

Separated, the interpretation becomes a pure function of a string, which is
the easiest kind of code to test exhaustively. The acquisition becomes a leaf
you choose:

```c
struct FileOps {
    char *(*read_all)(const char *path);
};
```

The real leaf opens, seeks, slurps, and returns a heap buffer. The fake
ignores `path` entirely and `strdup`s a string the test planted with
`file_fake_set`. `parse_config_count(const struct FileOps *io, const char
*path)` calls `io->read_all(path)` and never names `fopen`.

## The contract both leaves must honour

A seam is only as good as the agreement between its leaves, and the agreement
here is about ownership. Both `read_all` implementations return a buffer the
caller must `free`, or `NULL` on failure. Nothing else would work: the parser
has one code path, so it cannot free the fake's buffer and not the real one.

Get this wrong in either direction and the fake stops being a substitute. A
fake that returns a pointer to a static string will crash the parser's
`free`. A fake that never returns `NULL` means the failure branch is never
exercised, and you will discover it the first time a config file is missing
in production. The fake's job is to be indistinguishable to the code under
test — including in the ways that code can get it wrong.

That is why `file_fake_set(NULL)` exists. Producing a failed read from the
real leaf means arranging for a file to be absent or unreadable, which is
filesystem theatre; the fake just says no.

## Worked example

The test plants two `key=value` lines plus a comment and a blank line, and
expects 2. It plants a line with no `=` and expects 0. It plants `NULL` and
expects `-1`:

```text
$ make && ./prog; echo exit=$?

3 checks, 0 failed
exit=0
```

Three parse rules — skip comments, skip blanks, require `=` — exercised in
microseconds, with no file created, no directory assumed, and no cleanup to
forget. The same three would need three temp files and a teardown otherwise.

It is tempting to keep `fopen` inside the parser and write a temp file in the
test, on the grounds that it works and needs no new struct. It does work, and
for an integration test it is the right thing. It is wrong for *this* claim,
because you are then asserting on the filesystem and the parser at once: when
that test goes red you do not know which one broke, and when it goes green
you have not established that a seam exists at all. Keep both — a handful of
integration tests that really open files, and fast parse tests that do not.

## Distinctions worth keeping straight

- **Parser vs reader.** Interpret strings; inject how strings arrive.
- **Unit test vs integration test.** The fake proves parse rules; a real file
  proves the plumbing. Different tests, both worth having.
- **Fake returning `NULL` vs a missing file.** Same branch exercised, one of
  them on demand.
- **Ownership as part of the interface.** Who frees is as much the contract
  as the return type.

## Check yourself

1. What does the fake's `read_all` ignore, and why is that safe?
2. Who frees the buffer `read_all` returns, and what breaks if the two leaves
   disagree about it?
3. Why does `file_fake_set(NULL)` earn its place in the header?
4. Give one thing a temp-file test proves that the fake cannot.
5. Why does `parse_config_count` take a `const struct FileOps *` rather than
   just a `const char *` of already-read text?

## Key takeaways

- File I/O is a dependency; put it behind a struct and pass it in.
- Tests feed strings; production reads paths; the parser cannot tell.
- Both leaves must agree on ownership and on how failure is reported, or the
  substitution is a lie.
- A fake can produce failure states on demand that the real world only offers
  by accident.
- Injecting the reader does not retire integration tests — it stops them from
  being your only tests.

## Lookup (not the lesson)

- `man 3 fopen`, `man 3 strdup`, `man 3 strtok_r`
- `~/dev/c/LearnCwithRPG-terminal/code/ch24/config.c` — the direct `fopen`
- `~/dev/c/LearnCwithRPG-terminal/code/ch24/fuzz_parsers.c` — the same
  separation at a different scale

Now open `TASK.md` and do the practice.
