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

## Distinctions worth keeping straight

- **`_start` stack argv vs `main(argc, argv)`** — libc startup copies stack into C
  parameters; you are before that.
- **argv[0] vs argv[1]** — first token is the program name; user args follow.
- **Pointers vs characters** — `8(%rsp)` is a pointer to chars elsewhere on the stack
  (above the vectors).
- **Freestanding link** — `as`+`ld` or `gcc -nostdlib`.

## Check yourself

1. Where is `argc` relative to `%rsp` at `_start`?
2. Where is the pointer `argv[0]` stored, versus the characters of the string?
3. Why does `./argv0 foo` still print the program path as the first write?
4. How do you compute length without libc?

## Key takeaways

- The kernel passes args via a stack image at process entry.
- Freestanding code must parse that image explicitly.
- `argv[0]` is a pointer at `8(%rsp)`; strings live where those pointers refer.
- Raw `write` is enough to observe the result.

## Lookup (not the lesson)

- `man 2 write`, `man 2 exit`, `man 2 syscall`, `unistd_64.h`, `man 1 as`, `man 1 ld`

Now open `TASK.md` and do the practice.
