# 43 — SIGSEGV handler (Track D)

## Goal
Install a `sigaction` handler for `SIGSEGV`, deliberately NULL-deref, handle
it, and exit cleanly (no raw crash).

## Do
1. Install handler with `sigaction(SIGSEGV, ...)`.
2. In the handler: write a message (async-signal-safe: `write(2)` preferred
   over `printf`), then `_exit(0)` or set a flag carefully.
3. From `main`, dereference NULL after the handler is installed.
4. Confirm process exits 0 and prints your handler message.

## Success
`./prog` catches the fault and exits cleanly with a handler message.

## Refs
`man 2 sigaction`, `man 7 signal`, `man 2 write`, `man 2 getpid`, `man 3 exit`
