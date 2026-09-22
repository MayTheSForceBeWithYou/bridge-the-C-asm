# 51 — From `_start` to `main`

Your C program does not begin at `main`. The ELF entry is `_start` (often from
`crt1.o`), which eventually calls into libc startup — commonly `__libc_start_main` —
which then calls your `main` with `argc`, `argv`, and `envp`. This lesson teaches you
to recover that path from disassembly when no man page for `__libc_start_main` exists
on this machine.

## What this lesson asks of you

Disassemble the linked program, find `<_start>`, and write a hand trace of how control
reaches `main`, citing instruction lines you saw. Optional: gdb `break _start` and
`stepi` until `main`. Use `info libc` if helpful — not a fake man page.

## What startup is doing

Roughly:

1. Kernel jumps to the ELF entry (`_start`).
2. Startup aligns the stack, gathers argc/argv/envp from the initial image.
3. It calls a libc helper (`__libc_start_main` or similar) with function pointers
   including `&main`.
4. That helper finishes runtime init and calls `main`.
5. After `main` returns, exit processing runs.

Exact symbol names vary slightly with libc/version; trust *your* `objdump` output.

## How to look

```
objdump -d ./prog | grep -A40 '<_start>:'
```

Follow `call` targets. You may also see `.init` / constructors later; for this exercise
the spine to `main` is enough.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | hosted `./prog` |
| `objdump -d ./prog` | search `<_start>:` |
| follow `call` targets | toward `__libc_start_main@plt` (or similar) then `main` |
| optional gdb | `break _start` → `stepi` until `main` |

## Navigation: do not drown in CRT

```bash
objdump -d ./prog | less
# search: <_start>:
```

`_start` is short. The rejected wrong reading is: "the first `call` in `_start` is already
`main`." Usually an intermediate libc startup call sits in between.

## Decoding the `lea`/`call` toward `main` (shape)

You may see RIP-relative loads that materialize `&main` into a register before a `call`
to `__libc_start_main@plt`. Decode columns as in exercise 01/40: left = instruction
address; `disp(%rip)` = offset; `# abs <main>` = resolved target comment.

**Rejected wrong reading:** a RIP displacement is the instruction address of `main`.

## No man page for `__libc_start_main`?

Reason from *your* disassembly and `info libc`. The bytes you dumped are authoritative
for this binary. **Rejected wrong reading:** "without a man page I cannot know anything."

## Hosted vs freestanding (Track D)

Hosted: ELF entry `_start` → libc startup → `main`. Freestanding: your `_start` *is* the
program and must `exit` itself.

## Worked example

**The situation.** Your notes say: `_start` calls `__libc_start_main` with `main`'s
address in a register/argument slot; `__libc_start_main` later calls `main`.

**Step 1 — find `_start`.** It is short. The rejected wrong reading is: "the first
`call` in `_start` is already `main`." Usually an intermediate libc startup call sits
in between.

**Step 2 — identify the helper.** Match the PLT or direct symbol name in the dump.
There is no `man 3 __libc_start_main` here — reason from disassembly + `info libc`.
The rejected wrong reading is: "without a man page I cannot know anything." The bytes
you disassembled *are* the documentation for this binary.

**Step 3 — connect argc/argv.** Startup passes the stack-derived argc/argv into `main`
per the C ABI (`%rdi`, `%rsi`, …). Optional stepi confirms.

## Hand-trace template (fill with *your* addresses)

```text
ELF entry → <_start> @ _______
  call → <__libc_start_main@plt> @ _______
    (libc startup)
      call → <main> @ _______
```

Cite instruction lines you actually saw. Symbol names can vary slightly with libc
version — trust your dump.

## Optional gdb path

`break _start` → `stepi` until you enter `main`. Watch argc/argv appear in `%rdi`/`%rsi`
per the C ABI at the call to `main`.

## Distinctions worth keeping straight

- **ELF entry vs `main`** — entry is `_start`; `main` is a callback from libc.
- **Missing man page vs missing mechanism** — inspect the binary.
- **PLT call to startup vs your PLT to printf** — same PLT machinery, different symbols.
- **Freestanding `_start` (Track D) vs hosted `_start`** — hosted chains into libc;
  freestanding *is* your whole program.

## Deeper worked navigation (_start to main)

- Entry is _start; main is a callback.
- First call in _start is rarely main.
- Disassembly beats missing man pages.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| objdump <_start>: | short prologue |
| call target | __libc_start_main@plt or similar |
| main | called from libc startup |

### Ordered navigation moves

1. Find _start.
2. Identify helper call.
3. Hand-trace to main.
4. Optional stepi.

### Rejected wrong readings (keep beside the artifact)

- first call is main.
- no man page ⇒ unknowable.
- freestanding _start is the same chain.

### Tool-line decoding reminders

- entry symbol vs main
- PLT of startup vs printf
- argc/argv ABI at main

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (51)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - first call in _start is main
- gave up without man page
- confused freestanding with hosted

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

1. What symbol does the ELF header use as the entry point for a normal gcc binary?
2. Why might you not see a direct `call main` inside `_start`?
3. Where should you look if `__libc_start_main` has no local man page?
4. How do argc/argv get to `main` in the hosted world?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Hosted programs reach `main` through CRT/libc startup from `_start`.
- Disassembly is authoritative when manuals are missing.
- `__libc_start_main` (or equivalent) is the usual bridge.
- Track D freestanding work intentionally skips this chain.

## Field-decoding recap for exercise 51

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 objdump`, `info libc`, `man 5 elf`, `man 1 gdb`

Now open `TASK.md` and do the practice.
