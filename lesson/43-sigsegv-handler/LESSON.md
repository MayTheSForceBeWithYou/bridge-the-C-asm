# 43 — Catching SIGSEGV with sigaction

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

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` |
| `./prog; echo $?` | handler message + exit 0 |
| `grep -n sigaction prog.c` | install site |

## Decoding `struct sigaction` fields you must set

| Field / action | What to do here |
| -------------- | --------------- |
| `sa_handler` | pointer to your handler (simple form) |
| `sa_flags` | `0` unless you knowingly need a flag |
| `sa_mask` | `sigemptyset(&sa.sa_mask)` |
| call | `sigaction(SIGSEGV, &sa, NULL)` — check for `-1` |

**Rejected wrong reading:** `signal(2)` is identical and fine. This track standardizes on
`sigaction`.

## Async-signal-safe I/O (recognition rule)

Safe here: `write(2)`, `_exit(2)`. Unsafe: `printf`, `malloc`, most buffered stdio.

**Navigation:** if you need the list later, `man 7 signal-safety` — but the lesson's rule
is already: write a fixed message with `write`, then `_exit`.

## Why returning from the handler is wrong for a null store

The faulting instruction is typically re-executed after a normal handler return. For a
null store that means looping `SIGSEGV` forever. This lab exits from the handler.

**Rejected wrong reading:** "`printf` the address and `return` to continue." Unsafe I/O
plus re-fault.

## Keeping the compiler from deleting the fault

Use `volatile` on the null access (or an equivalent that survives `-O2`). Without it the
compiler may remove a "useless" store and your handler never runs.

## What success looks like at the shell

| Observation | Meaning |
| ----------- | ------- |
| message from `write` | handler ran |
| `$?` is 0 | `_exit(0)` from handler |
| no endless reprint loop | you did not return into the fault |

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

## `_exit` versus `exit` in a handler

Prefer `_exit` / `_Exit` from the handler. `exit` runs `atexit` hooks and stdio teardown
that are not async-signal-safe. **Rejected wrong reading:** "`exit(0)` is always the
polite way to leave."

## Deliberate fault shape

```c
*(volatile int *)0 = 1;
```

`volatile` defeats "useless store" deletion at `-O2`. Install the handler *before* the
fault. **Rejected wrong reading:** "if the handler printed, null deref became defined
behavior." You are studying Linux delivery, not blessing null stores in production.

## Distinctions worth keeping straight

- **`sigaction` vs `signal`** — prefer `sigaction` for portable control.
- **Async-signal-safe vs ordinary libc** — `write` yes; `printf`/`malloc` no.
- **Handler return vs exit** — faulting memory ops usually need terminate or repair +
  careful resume; this lab terminates.
- **UB in the C abstract machine vs observed Linux delivery** — the course studies the
  machine you have; do not ship null derefs in real code.

## Deeper worked navigation (SIGSEGV handler)

- `sigaction` over `signal`.
- `write`+`_exit` in handler.
- Do not return into a null store.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| sigaction return | 0 success; -1 fail before faulting |
| handler message | from write(2) |
| exit status | 0 from `_exit` |

### Ordered navigation moves

1. Install+check.
2. volatile null store.
3. write message.
4. _exit(0).

### Rejected wrong readings (keep beside the artifact)

- printf in handler is fine.
- return resumes cleanly after null store.
- signal(2) is identical.

### Tool-line decoding reminders

- sa_handler vs sa_sigaction
- signal-safety: write yes, printf no
- volatile keeps the fault

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (43)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - printf in handler
- returned from handler into null store
- used signal(2) casually

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

1. Why is `printf` a bad idea inside a `SIGSEGV` handler?
2. What happens if the handler returns after a null store?
3. Which man section discusses signal-safe functions?
4. How do you keep the compiler from optimizing away the null access?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- `SIGSEGV` is how Linux presents many invalid memory accesses to user space.
- `sigaction` installs the handler; async-signal-safe I/O reports the event.
- Do not return into a still-faulting instruction without a real recovery plan.
- Clean exit from the handler is an acceptable lab resolution.

## Field-decoding recap for exercise 43

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 sigaction`, `man 7 signal`, `man 2 write`, `man 2 getpid`, `man 3 exit`

Now open `TASK.md` and do the practice.
