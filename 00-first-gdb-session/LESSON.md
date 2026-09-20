# 00 — A first gdb session
<!-- concepts: c.p5.gdb.symbols -->

Every later debugger exercise in this track assumes you can stop a running
program, look at a local, and take one source line at a time. After this lesson
you will open gdb on a tiny binary, set a breakpoint, watch one integer change
across a loop, and leave without guessing which command did what.

## What this lesson asks of you

Build `count`, start gdb the quiet way shown below, and drive a short session
with only eight commands: `break`, `run`, `next`, `step`, `print`,
`info locals`, `continue`, `quit`. No memory dumps, no disassembly, no register
windows — those arrive in exercises 32–39. Here the whole skill is *stop, look,
advance one line, look again*.

`TASK.md` is the practice. This file is the teaching.

## Starting gdb without the noise

On this machine two things pollute a first session. Suppress both up front:

```bash
gdb -q -iex 'set debuginfod enabled off' ./count
```

`-q` drops the licence banner. The `-iex` line turns off debuginfod so gdb does
not ask *"Enable debuginfod for this session?"* on first run. That prompt is
real; it is also not a decision you need an opinion about yet. The rest of this
lesson shows only what gdb printed in response to commands you type.

Addresses and the exact breakpoint hex will differ every run (ASLR). Treat them
as placeholders, not as values to memorize.

## The eight commands, named for what they do

| Command | Job |
| ------- | --- |
| `break main` (or `break 9`) | Plant a stop at a function or source line |
| `run` | Start the inferior from the beginning |
| `next` | Execute the current source line; do not enter callees |
| `step` | Execute the current source line; enter a callee when you can |
| `print total` | Evaluate an expression in the current frame |
| `info locals` | List locals gdb can see in the current frame |
| `continue` | Run until the next breakpoint or exit |
| `quit` | Leave gdb (it may ask if the inferior is still alive) |

`next` and `step` look identical on a line that is not a call. On a call, `next`
finishes the call and stops on the following line of *your* function; `step`
tries to enter the callee. If the callee has no debug symbols loaded — typical
for libc `printf` with debuginfod off — both may land on the next line of your
source. The distinction still matters; you will feel it the first time you
`step` into *your own* helper.

## Worked example

Program under test: `count.c` sums `1..5` into `total` and prints it. Build with
`make`, then start gdb as above. A real session on this machine looked like
this (addresses will not match yours):

```text
Reading symbols from ./count...
(gdb) break main
Breakpoint 1 at 0x1141: file count.c, line 5.
(gdb) run
Starting program: .../00-first-gdb-session/count
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/usr/lib/libthread_db.so.1".

Breakpoint 1, main () at count.c:5
5	    int total = 0;
(gdb) info locals
total = 0
i = 0
(gdb) next
8	    for (i = 1; i <= 5; i++) {
(gdb) next
9	        total += i;
(gdb) print i
$1 = 1
(gdb) print total
$2 = 0
(gdb) next
8	    for (i = 1; i <= 5; i++) {
(gdb) print total
$3 = 1
(gdb) continue
Continuing.
total=15
[Inferior 1 (process ...) exited normally]
(gdb) quit
```

Read that as a story. You stopped *before* `total = 0` finished; `info locals`
still showed both locals at zero (uninitialized `i` happens to read as 0 here —
do not depend on that). Two `next`s put you on the first `total += i` with
`i == 1` and `total` still 0. One more `next` completed the add; `total` became
1. `continue` ran the rest of the loop and the `printf`, then the process
exited normally.

It is tempting to read `info locals` at the first stop as "the initializer
already ran." That is wrong, because `break main` skips the function prologue
and plants the stop on the first *statement* line — here line 5, the
initializer itself. You are stopped at that line, about to execute it, not
after it. The `total = 0` you see is uninitialized stack that happens to read
as zero, which is exactly why it fools people. Always `print` again after
`next` when the claim you care about is "this assignment happened."

## Distinctions worth keeping straight

- **`break` plants; `run` starts.** A breakpoint with no `run` does nothing.
- **`next` vs `step`.** Same on non-calls; different on calls when the callee
  has symbols.
- **`print` vs `info locals`.** One expression versus the whole visible set.
- **`continue` vs another `next`.** Resume until the next stop, versus one line.
- **Process exit is normal.** `exited normally` after `continue` means the
  program finished; it is not a debugger failure.

## Check yourself

1. Why start with `gdb -q -iex 'set debuginfod enabled off'` on this machine?
2. You hit `break main`, `run`, and `info locals` shows `total = 0`. Has the
   initializer definitely executed?
3. When do `next` and `step` disagree in an interesting way?
4. You are on `total += i` with `i == 1` and `total == 0`. What does one `next`
   followed by `print total` show?
5. What does `continue` do that repeated `next` does not?

## Key takeaways

- A first gdb session is stop, inspect, advance — eight commands cover it.
- Breakpoints land *on* a line; confirm side effects with `print` after `next`.
- `next` stays in your function across calls; `step` enters when symbols allow.
- Addresses in gdb output are not stable across runs under ASLR.
- Later Track C exercises add memory, stepi, and registers; they assume this.

## Lookup (not the lesson)

- `man 1 gdb` — command names and flags
- gdb `help break`, `help next`, `help step`, `help print` — spelling only

Now open `TASK.md` and do the practice.
