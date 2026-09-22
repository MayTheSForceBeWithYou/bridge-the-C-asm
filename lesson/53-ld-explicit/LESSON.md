# 53 — Linking with an explicit `ld` line

The `gcc` driver hides a lot of linker flags. For freestanding programs you often
invoke `ld` yourself — and every flag should be a choice you can defend. This lesson
forces an explicit link line for a tiny `_start` that writes and exits, then compares
what `gcc -nostdlib -v` would have passed.

## What this lesson asks of you

Write `start.s` with `_start` that `write`s a short message and `exit`s. Assemble with
`as`, link with an explicit `ld -o prog start.o` (plus any flags you discover you need).
Document each flag. Diff against `gcc -nostdlib -v` output.

## Minimal freestanding link

On x86-64 Linux, assembling with `as --64 -g -o start.o start.s` and linking with
`ld -o prog start.o` often works for a pure syscall program: you provide `_start`, you
need no libc, and the default ELF entry is `_start`. Architecture flags, `-m elf_x86_64`,
or dynamic interpreter options appear when you leave freestanding pure-static territory.

If the Makefile's `explicit` target is a sketch, edit until `./prog` prints and exits 0.
The pedagogy is ownership of the link line, not memorizing every GNU ld option.

## Reading gcc -v

`gcc -nostdlib -v start.s -o prog` prints the `collect2`/`ld` invocation. Note extra
paths (`-L`), startup objects (hopefully absent with `-nostdlib`), and machine flags.
Ask: which of those does your manual line truly need?

## Mapping `make` / explicit commands

| You type | Check afterward |
| -------- | --------------- |
| `as --64 -o start.o start.s` | `nm start.o` shows `_start` |
| `ld -o prog start.o` (+ only needed flags) | `./prog` prints and exits 0 |
| `gcc -nostdlib -v start.s -o prog.gcc` | compare printed `ld` line to yours |

## Entry symbol recognition

Default `ld` entry is `_start`. **Rejected wrong reading:** the entry must be named
`main`. `main` is a C/CRT convention.

## Driver vs linker

`gcc` orchestrates; `ld` links. Ordinary `gcc foo.s` without `-nostdlib` pulls CRT/libc
and expects `main`. **Rejected wrong reading:** that is "the same" as explicit freestanding
`ld`.

## Flag archaeology discipline

Start from `ld -o prog start.o`. Add only what fails without. Document each flag you keep.
**Rejected wrong reading:** paste twenty `gcc -v` flags without knowing which matter.

## Reading `gcc -v`

**Navigation:** search the verbose output for `collect2` or `ld`. Note `-L` paths,
startup objects (should be absent with `-nostdlib`), and machine flags — then ask which
your manual line truly needs.

## Worked example

**The situation.** `./prog` prints `hi\n` and exits 0. Your notes list `ld` flags you
passed and one-line purposes.

**Step 1 — assemble.** Object exists with `_start` symbol (`nm start.o`). The rejected
wrong reading is: "`gcc start.s -o prog` without `-nostdlib` is the same as explicit
freestanding ld." Without `-nostdlib`, gcc pulls CRT/libc and expects `main`.

**Step 2 — link explicitly.** Start from `ld -o prog start.o`. Add only what fails
without. The rejected wrong reading is: "copy-paste twenty gcc -v flags without knowing
which matter." Document only what you use.

**Step 3 — compare verbose gcc.** Identify hidden flags; keep your line minimal.

## Minimal freestanding `ld` line that often works

```bash
as --64 -o start.o start.s
ld -o prog start.o
./prog
```

Add `-m elf_x86_64` or other flags only if your platform `ld` requires them. Document
each addition.

## Comparing to `gcc -nostdlib -v`

**Navigation:** in verbose output, find the `ld`/`collect2` line. Ask which of those
paths/objects you truly need for a syscall-only `_start`. Copy only with understanding.

## Distinctions worth keeping straight

- **Driver vs linker** — gcc orchestrates; ld performs the link.
- **`-nostdlib` vs freestanding asm** — both avoid libc; entry/`main` expectations
  differ.
- **Flag archaeology vs mechanism** — learn why a flag exists when you add it.
- **Static syscall binary vs dynamic** — explicit ld freestanding is usually static
  with no INTERP.

## Deeper worked navigation (explicit ld)

- Own the ld line; start minimal.
- gcc -v reveals a longer taste — copy with understanding.
- Entry default _start.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| nm start.o | _start present |
| ld -o prog start.o | runs |
| gcc -nostdlib -v | compare flags |

### Ordered navigation moves

1. Assemble.
2. Link minimal.
3. Document each added flag.
4. Diff vs gcc -v.

### Rejected wrong readings (keep beside the artifact)

- entry must be main.
- paste all gcc -v flags blindly.
- gcc foo.s without nostdlib is freestanding.

### Tool-line decoding reminders

- driver vs ld
- -nostdlib meaning
- flag purpose notes

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (53)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - pasted gcc -v wholesale
- expected main as entry
- gcc without -nostdlib as freestanding

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

1. What entry symbol does ld expect by default for your program?
2. Why does ordinary `gcc foo.s` without `-nostdlib` fail for a `_start`-only file that
   never defines `main`?
3. What is the educational goal of writing the `ld` line by hand?
4. Where do you look up a flag you do not recognize?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Explicit `ld` makes linker policy visible.
- Freestanding syscall programs can link with a short flag list.
- `gcc -v` reveals the driver's longer taste; copy only with understanding.
- Documenting each flag is part of the competence.

## Field-decoding recap for exercise 53

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 ld`, `info ld`, `man 1 as`, `man 1 gcc`, `man 2 write`, `man 2 exit`

Now open `TASK.md` and do the practice.
