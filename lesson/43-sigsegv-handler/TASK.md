# 43 — SIGSEGV handler (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. Install a `sigaction` handler for `SIGSEGV`.
2. In the handler: async-signal-safe message (`write(2)`), then exit cleanly (e.g. `_exit(0)`).
3. From `main`, dereference NULL after the handler is installed.
4. Confirm exit 0 and the handler message.

## Done when

- `./prog` catches the fault, prints your message, and exits cleanly.

## Lookup

`man 2 sigaction`, `man 7 signal`, `man 2 write`, `man 2 getpid`, `man 3 exit`.
