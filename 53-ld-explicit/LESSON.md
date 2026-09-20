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

## Distinctions worth keeping straight

- **Driver vs linker** — gcc orchestrates; ld performs the link.
- **`-nostdlib` vs freestanding asm** — both avoid libc; entry/`main` expectations
  differ.
- **Flag archaeology vs mechanism** — learn why a flag exists when you add it.
- **Static syscall binary vs dynamic** — explicit ld freestanding is usually static
  with no INTERP.

## Check yourself

1. What entry symbol does ld expect by default for your program?
2. Why does ordinary `gcc foo.s` without `-nostdlib` fail for a `_start`-only file that
   never defines `main`?
3. What is the educational goal of writing the `ld` line by hand?
4. Where do you look up a flag you do not recognize?

## Key takeaways

- Explicit `ld` makes linker policy visible.
- Freestanding syscall programs can link with a short flag list.
- `gcc -v` reveals the driver's longer taste; copy only with understanding.
- Documenting each flag is part of the competence.

## Lookup (not the lesson)

- `man 1 ld`, `info ld`, `man 1 as`, `man 1 gcc`, `man 2 write`, `man 2 exit`

Now open `TASK.md` and do the practice.
