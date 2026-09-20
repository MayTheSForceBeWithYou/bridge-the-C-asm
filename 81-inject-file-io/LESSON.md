# 81 — Inject file I/O
<!-- concepts: c.p4.di.fileops, c.p4.di.fake -->

A config parser that calls `fopen` itself can only be tested with real
files (or heroic `chroot` tricks). Lift the read behind
`struct FileOps { char *(*read_all)(const char *); }` and a test can feed
a string. Your `ch24/config.c` still opens the path directly; your fuzz
harness already invents bytes — this lesson names the seam that makes
both honest.

## What this lesson asks of you

Run the fake-fed parser test. Read how `parse_config_count` never calls
`fopen`. Optionally implement `student_parse.c`. Harvest mental model from
`ch24/config.c` and `ch24/fuzz_parsers.c` (read-only).

## Feed the parser a string

```c
struct FileOps {
    char *(*read_all)(const char *path);
};
```

Real leaf: open, slurp, malloc, return. Fake leaf: ignore `path`,
`strdup` a string the test planted with `file_fake_set`. Parser frees
whatever `read_all` returned. Same ownership rule for both leaves — that
is the contract.

## Worked example

The shipped test plants two `key=value` lines plus a comment and blank
line. `parse_config_count` returns 2. A second plant with no `=` returns
0. It is tempting to keep `fopen` inside the parser and "just write a
temp file in the test." That is wrong for this lesson's claim, because
you are still testing the filesystem and permissions, not only the parse
rules — and you have not proved a seam exists.

## Distinctions worth keeping straight

- **Parser vs reader.** Parse strings; inject how strings arrive.
- **Fuzz bytes vs unit-test strings.** Same idea at different scales.
- **Temp files vs `FileOps`.** Possible, heavier, hides the dependency.

## Check yourself

1. What does the fake's `read_all` ignore?
2. Who frees the buffer `read_all` returns?
3. Why does `ch24/config.c`'s direct `fopen` make pure parse tests harder?
4. Name one benefit shared with the clock and renderer seams.

## Key takeaways

- File I/O is a dependency; put it in a struct.
- Tests feed strings; production reads paths.
- Ownership must be identical across leaves.

## Lookup (not the lesson)

- `man 3 fopen`, `man 3 strdup`
- `ch24/config.c`, `ch24/fuzz_parsers.c`

Now open `TASK.md` and do the practice.
