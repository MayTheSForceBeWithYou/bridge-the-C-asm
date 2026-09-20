# 43 — Catching SIGSEGV with sigaction
<!-- concepts: c.p5.os.sigsegv -->

A null dereference is not "C undefined behavior happening in the abstract" — on Linux
it is a hardware fault delivered as the signal `SIGSEGV`. You can install a handler with
`sigaction`, print a last message using only async-signal-safe calls, and exit cleanly
instead of dumping core. This lesson is about that delivery path and its constraints.

## What this lesson asks of you

Install a `SIGSEGV` handler, deliberately dereference NULL from `main`, handle the
fault, and leave the process with exit status 0 and a visible handler message. You
should know why `printf` in a handler is unsafe and what to use instead.

## Signals and sigaction

`sigaction(2)` registers a handler function (or SA_SIGINFO variant) for a signal number.
When the kernel delivers `SIGSEGV`, your process is interrupted and the handler runs on
a signal stack frame. Returning from a handler after a faulting memory access is often
wrong — the faulting instruction would retry — so for this lab you terminate from the
handler with `_exit` / `exit` after writing a message.

`man 7 signal` lists async-signal-safe functions. `write(2)` to stderr or stdout is the
usual safe print. `printf` may take locks libc already held when the fault occurred —
deadlock or corruption risk.

## The deliberate fault

After the handler is installed, `*(volatile int *)0 = 1;` (or a load) triggers the
signal. `volatile` keeps the compiler from deleting a "useless" store. Without a
handler, you would see a crash; with one, you control the goodbye.

## Worked example

**The situation.** `./prog` prints something like `caught SIGSEGV` and exits 0 — no
shell "Segmentation fault" line, or at least the process status is clean success as the
harness expects.

**Step 1 — install.** Fill a `struct sigaction`, set `sa_handler` (or `sa_sigaction`),
clear or set masks as needed, call `sigaction(SIGSEGV, &sa, NULL)`. Check the return.
The rejected wrong reading is: "signal(2) is fine and identical." `signal`'s semantics
vary; this track standardizes on `sigaction`.

**Step 2 — fault.** Null-deref from `main`. The rejected wrong reading is: "the handler
can `printf` the address and `return` to continue the program." Returning re-executes
the faulting instruction; for a null deref that loops the signal. Exit from the handler
(or longjmp only if you deeply understand the rules — not required here).

**Step 3 — message.** `write(2, msg, len)` then `_exit(0)` (or set a flag and exit from
main only if you unblock carefully — simplest is exit in the handler for this exercise).

## Distinctions worth keeping straight

- **`sigaction` vs `signal`** — prefer `sigaction` for portable control.
- **Async-signal-safe vs ordinary libc** — `write` yes; `printf`/`malloc` no.
- **Handler return vs exit** — faulting memory ops usually need terminate or repair +
  careful resume; this lab terminates.
- **UB in the C abstract machine vs observed Linux delivery** — the course studies the
  machine you have; do not ship null derefs in real code.

## Check yourself

1. Why is `printf` a bad idea inside a `SIGSEGV` handler?
2. What happens if the handler returns after a null store?
3. Which man section discusses signal-safe functions?
4. How do you keep the compiler from optimizing away the null access?

## Key takeaways

- `SIGSEGV` is how Linux presents many invalid memory accesses to user space.
- `sigaction` installs the handler; async-signal-safe I/O reports the event.
- Do not return into a still-faulting instruction without a real recovery plan.
- Clean exit from the handler is an acceptable lab resolution.

## Lookup (not the lesson)

- `man 2 sigaction`, `man 7 signal`, `man 2 write`, `man 2 getpid`, `man 3 exit`

Now open `TASK.md` and do the practice.
