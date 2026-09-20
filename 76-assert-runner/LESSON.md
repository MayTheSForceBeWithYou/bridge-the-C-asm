# 76 — Build the assert runner you have been using
<!-- concepts: c.p3.test.check, c.p3.test.exitstatus -->

In Apex a test framework swallows the mechanics: green bar, stack trace,
pass count. In C you own those mechanics yourself — usually as a few
macros and two integers. After this lesson you can explain why
`__FILE__`/`__LINE__` appear in a failure line, why the process exit
status is the whole CI contract, and why "a test" is just a function
calling a function and comparing.

## What this lesson asks of you

Write your own tiny runner in `my_check.h` (counters, `CHECK`,
`CHECK_EQ`, `test_report`). Run the shipped `prog` that already uses
`../test/check.h` so you see a deliberate failure print a file:line and
exit 1. Then diff your header against the harvested one. The point is
not inventing a framework; it is seeing that the framework was never
magic.

`TASK.md` is practice. This file is the teaching.

## A test is a function calling a function

Strip the ceremony. A unit test does three things:

1. Call the code under test with a known input.
2. Compare the result to an expected value.
3. Remember whether that comparison failed.

Everything else — pretty names, fixtures, parallel runners — is
packaging. Your own RPG already ships a header that does exactly those
three things in about twenty lines. Apex hid the same three behind a
runner you never opened. That hiding is the thing being demystified
here: once you can write the macros, the IDE green bar is just a UI
over an exit status.

## Why `__FILE__` and `__LINE__` are preprocessor facts

When the preprocessor expands `CHECK(x > 0)` at line 42 of `prog.c`, it
pastes the tokens of the macro body into that location. `__FILE__` and
`__LINE__` are replaced *at the expansion site*, not inside the header
where the macro was defined. That is why a failure prints `prog.c:42`
instead of `check.h:17`. The cost is real: every check site embeds a
string literal for the file name and the expression text (`#expr`), so
the binary grows with the number of checks. For a teaching suite that
cost is noise; for millions of checks you would care.

## Exit status is the integration contract

`test_report` returns non-zero when `tests_failed != 0`. `main` returns
that value. The shell, `make`, and CI all agree on one rule: exit 0
means pass, anything else means fail. No XML required. If your runner
prints FAIL lines but always `return 0`, every automated gate will lie
that you are green. The printout is for humans; the status is for the
pipeline.

## Worked example

Build and run the shipped demo:

```text
$ make -C 76-assert-runner
$ ./76-assert-runner/prog; echo exit=$?
  FAIL prog.c:14: 1 + 1 == 2, expected 3

5 checks, 1 failed
exit=1
```

Five checks ran; the last `CHECK_EQ(1 + 1, 3)` is intentional. The FAIL
line names this file and the line of the macro *invocation*, and prints
the actual value (`2`) beside the expected one (`3`). Exit status 1 is
`test_report` doing its job.

It is tempting to read a header full of macros as "a miniature JUnit."
That is wrong, because there is no discovery, no class loader, and no
annotation processor — only textual expansion and two static ints. If
you can expand `CHECK` by hand on paper, you understand the whole
framework.

## Distinctions worth keeping straight

- **Human FAIL text vs machine exit status.** Both required; neither
  replaces the other.
- **Macro expansion site vs macro definition site.** `__LINE__` is about
  the call.
- **Your `my_check.h` vs `../test/check.h`.** Same job; the harvested
  file also has `CHECK_STR` / `CHECK_PTR`.
- **Apex runner vs C macros.** Same three responsibilities; different
  opacity.

## Check yourself

1. Why does a failure print `prog.c:N` rather than a line inside
   `check.h`?
2. What does `test_report` return when every check passed?
3. If FAIL lines print but `main` always returns 0, what breaks?
4. Name the three responsibilities of this "framework."
5. Why is `#expr` in the `CHECK` printf useful?

## Key takeaways

- A C test runner can be counters plus macros; the rest is optional.
- `__FILE__`/`__LINE__` bind at the expansion site — that is the whole
  trick behind useful failure lines.
- Exit status is the contract `make` and CI actually enforce.
- Apex hid these mechanics; writing them once removes the mystery.
- Diffing your header against `../test/check.h` is the lab, not a
  judgment of style.

## Lookup (not the lesson)

- `man 3 printf` — format specifiers
- `info cpp` — `__FILE__`, `__LINE__`, stringification `#`

Now open `TASK.md` and do the practice.
