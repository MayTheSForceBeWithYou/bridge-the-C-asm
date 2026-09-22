# 45 — Reading argv from the initial stack

When the kernel enters your freestanding `_start`, `%rsp` already points at a defined
layout: argument count, then pointers to argument strings, then a NULL, then environment
pointers. No libc parses that for you. This lesson teaches you to pull `argv[0]` off
that stack and `write` it to stdout.

## What this lesson asks of you

Implement `_start` that loads `argc` from `(%rsp)`, loads `argv[0]` from `8(%rsp)`,
computes the string length, writes the bytes plus a newline, and exits 0 — freestanding.

## The initial stack layout (x86-64 Linux)

At entry:

| Offset from `%rsp` | Content |
| ------------------ | ------- |
| 0 | `argc` (8-byte integer) |
| 8 | `argv[0]` pointer |
| 16 | `argv[1]` pointer (if argc > 1) |
| … | … |
| 8*(argc+1) | NULL terminator of argv |
| then | `envp[0]…` NULL-terminated |

`argv[0]` is conventionally the program path/name as invoked. Running `./argv0` versus
`./argv0 foo` changes `argc` and later slots; slot 0 still names the program.

## Length without libc

Walk bytes until you see 0. That count is the `write` length. Then write `"\n"` as a
second call or include it if you prefer a tiny static newline buffer.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | freestanding `./argv0` |
| `./argv0` | program name + newline |
| `./argv0 foo` | still prints program name first |
| `nm ./argv0` | no libc `strlen`/`printf` |

## Initial stack layout (decode the table, do not skim)

| Offset from `%rsp` | Content |
| ------------------ | ------- |
| 0 | `argc` |
| 8 | `argv[0]` pointer |
| 16 | `argv[1]` (if present) |
| `8*(argc+1)` | NULL end of argv |
| then | `envp[]` NULL-terminated |

**Rejected wrong reading:** argc/argv arrive in `%rdi`/`%rsi` at raw `_start` like
`main`. The kernel puts them on the stack; libc startup later copies into C ABI regs.

## Pointers versus characters

| Action | What you get |
| ------ | ------------ |
| `mov 8(%rsp), %rsi` | pointer to chars |
| `write` using that pointer + strlen | the characters |
| `write` of the 8 bytes at `8(%rsp)` | garbage address bytes |

**Rejected wrong reading:** writing the qword at `8(%rsp)` prints argv[0].

## Optional gdb navigation

```text
(gdb) break _start
(gdb) run
(gdb) x/8gx $rsp
(gdb) x/s *(char**)($rsp+8)
```

First qword = argc; second = pointer; `x/s` shows the string.

## Worked example

**The situation.** `./argv0` prints a path ending in `argv0` (or similar) and a newline.
`./argv0 foo` still prints the program name first — not `foo`.

**Step 1 — load argc and argv0.** `mov (%rsp), %rdi`-style for argc if you need it;
`mov 8(%rsp), %rsi` for the pointer to pass to `write`. The rejected wrong reading is:
"argv lives in registers at entry like the C ABI for `main`." The kernel does not
prefill `%rdi`/`%rsi` with argc/argv for raw `_start`; the stack does.

**Step 2 — strlen.** Byte loop; do not call libc `strlen`. The rejected wrong reading is:
"argc's value is the length of argv[0]." `argc` is the count of pointers, not a string
length.

**Step 3 — write and exit.** `__NR_write` (1) to fd 1; `__NR_exit` (60). Confirm with
`nm` there is no libc.

## Length without libc (byte walk)

```text
len = 0
while byte_at(argv0 + len) != 0: len++
write(1, argv0, len)
write(1, "\n", 1)   # or include newline another safe way
exit(0)
```

**Rejected wrong reading:** "`argc` is the length of `argv[0]`." `./argv0 foo` has
`argc==2` while the program name string length is unrelated.

## Freestanding link reminder

`as`+`ld` or `gcc -nostdlib`. Confirm with `nm` / `readelf -d` as in exercise 40.

## Distinctions worth keeping straight

- **`_start` stack argv vs `main(argc, argv)`** — libc startup copies stack into C
  parameters; you are before that.
- **argv[0] vs argv[1]** — first token is the program name; user args follow.
- **Pointers vs characters** — `8(%rsp)` is a pointer to chars elsewhere on the stack
  (above the vectors).
- **Freestanding link** — `as`+`ld` or `gcc -nostdlib`.

## Deeper worked navigation (argv on entry stack)

- argc at (%rsp); argv[0] ptr at 8(%rsp).
- Write characters, not the pointer bytes.
- argc is not strlen(argv[0]).

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| x/gx $rsp | argc then pointers |
| x/s argv0 | characters |
| ./argv0 foo | still prints program name first |

### Ordered navigation moves

1. Load 8(%rsp).
2. strlen bytewalk.
3. write+newline.
4. exit.

### Rejected wrong readings (keep beside the artifact)

- argv arrives in %rdi/%rsi at raw _start.
- write the qword at 8(%rsp).
- argc is the string length.

### Tool-line decoding reminders

- stack table offsets
- pointer vs string
- freestanding nm/readelf checks

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (45)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - treated argc as strlen
- wrote pointer qword as text
- assumed argv in %rdi at _start

    For each tick: write the *recognition* fix (which register/column/anchor) in one line.
    That diary is how this track sticks.

    ## Makefile → command → file (recap)

    | You type | Produces / runs | Open next |
    | -------- | --------------- | --------- |
    | `make` / `make bin` | exercise binary | run it; note exit status |
    | `make clean` | removes objects | before changing `O=` / flags |
    | `make asm` / `make disasm` (if any) | listing view | search the label you care about |
    | tools in Lookup | field dumps | decode columns, do not skim blobs |

    Remember: a disasm target usually *views* bytes already linked — it is not a new
    mysterious compile stage (lesson 01's `.lst` rule).

## Check yourself

1. Where is `argc` relative to `%rsp` at `_start`?
2. Where is the pointer `argv[0]` stored, versus the characters of the string?
3. Why does `./argv0 foo` still print the program path as the first write?
4. How do you compute length without libc?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- The kernel passes args via a stack image at process entry.
- Freestanding code must parse that image explicitly.
- `argv[0]` is a pointer at `8(%rsp)`; strings live where those pointers refer.
- Raw `write` is enough to observe the result.

## Field-decoding recap for exercise 45

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 write`, `man 2 exit`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
