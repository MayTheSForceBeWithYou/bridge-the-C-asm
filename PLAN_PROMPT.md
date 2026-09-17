# Prompt for grok bot: build `c-and-asm-grind`

Paste everything below this line to grok.

---

You are setting up a learn-by-doing exercise track in `/home/n8/dev/c/c-and-asm-grind`. The goal is to make me write a LOT of C and x86-64 assembly by hand — not read about it. Every exercise is something I type in, compile, run, and inspect myself. You are building the exercise scaffolding and instructions, not the solutions.

## Environment (already confirmed on this machine)

- Linux x86_64, System V AMD64 ABI, AT&T syntax by default.
- `gcc` (GCC 16), GNU `as`/`ld` (binutils 2.47), `objdump`, `gdb` 17 are all installed.
- `nasm` is NOT installed — do not depend on Intel-syntax nasm exercises unless a setup step installs it first. Default to GNU `as` (AT&T syntax) and `gcc -S` output. If you want Intel syntax for readability, use `objdump -d -M intel` or `gcc -S -masm=intel`, not nasm.
- No existing build system here — keep it to plain Makefiles, one per exercise directory or one umbrella Makefile with pattern rules. Don't introduce cmake/meson/etc.

## Reference material already in `/home/n8/dev/c/`

Look at these sibling folders for tone, level, and prior art before designing exercises — don't duplicate what they already cover well:

- `BareMetalC/` — already has the exact C→preprocess→asm→object→binary pipeline for one hello world (`.c`, `.i`, `.s`, `.o`, `.lst`, binary, tiny Makefile). Use this as the template for the "inspect every compilation stage" exercises, but generalize it into a reusable pattern instead of one-off files.
- `hard-way/`, `LearnCbyGrinding/`, `EffectiveC/`, `UdemyCProgrammingForBeginners/`, `LearnCwithRPG-graphics/`, `LearnCwithRPG-terminal/`, `walkthrough-CRPG/` — these are other C learning tracks at various levels (C fundamentals, a text RPG, a graphical RPG). Skim them to gauge what C concepts I've likely already drilled, so `c-and-asm-grind` can start past total-beginner C and lean hard into the assembly side and the C/asm relationship, which none of the others cover.
- `cmake-tutorial/` — unrelated build-system track, ignore for content but note it exists so you don't collide on naming.

## What to produce

Create a directory tree under `/home/n8/dev/c/c-and-asm-grind/` with:

1. A top-level `README.md` explaining the structure, how to build/run each exercise, and the learning philosophy (below).
2. A top-level `Makefile` (or per-exercise Makefiles) with standard targets: build the C source, emit `.i` (preprocessed), `.s` (assembly, AT&T), `.o` (object), a linked binary, and a disassembly listing (`objdump -d`). Same shape as `BareMetalC/hello.c`'s pipeline, but parameterized so it works for every exercise, not hand-copied per file.
3. Numbered exercise directories (e.g. `01-hello-pipeline/`, `02-stack-frames/`, ...) each containing:
   - A short `TASK.md` (a few sentences: what to write, what to observe, what question to answer by looking at the assembly — not a tutorial, a prompt).
   - A starter `.c` file with either nothing but a signature/TODO, or deliberately minimal scaffolding — I write the real logic.
   - For the assembly-from-scratch exercises: a starter `.s` file (or none) plus a `TASK.md` describing the routine to hand-write and the C harness that calls it to test it.
   - No solution files. No answer keys. If a concept needs explaining, point at the specific `man`/`info` page or GCC/binutils doc section to read, don't explain it inline.

## Exercise design requirements

Mix of two tracks, interleaved by difficulty rather than fully separated:

### Track A: C → assembly (write C, read/compile to asm, learn from what the compiler emitted)
Progression should force me to actually look at `.s` / `objdump -d` output and answer a specific question each time, e.g.:
- How does a simple function's prologue/epilogue look, and what changes with `-O0` vs `-O2`?
- Where do local variables live (stack offsets) vs registers, and how does that shift under optimization?
- How are function arguments passed (System V AMD64 ABI: rdi/rsi/rdx/rcx/r8/r9, stack for overflow)?
- Structs: field layout, padding/alignment, and how member access compiles to offset loads.
- Arrays vs pointers: indexing vs pointer arithmetic in the emitted asm — are they actually identical?
- Loops: `for`/`while`/`do-while` — how do the generated jumps/labels differ?
- Recursion (e.g. factorial, fibonacci): stack growth, saved registers, tail-call cases with `-O2`.
- Switch statements: jump tables vs cascaded compares, and when GCC picks which.
- Function pointers and calling through them.
- Global vs local vs static variables: where each lives (`.data`/`.bss`/`.rodata`/stack) — inspect with `objdump -h`/`nm`/`size`.
- String literals and how they land in `.rodata`.
- A small struct-heavy or linked-list program: pointer chasing in asm.
- Integer overflow / signed vs unsigned comparison: what instructions differ (`jl` vs `jb`, etc).
- Floating point: SSE registers (`xmm0`...) instead of general-purpose registers, calling convention differences.
- Inline asm (`asm volatile`) as a bridge exercise between the two tracks.

### Track B: Assembly from scratch (hand-write `.s`, assemble with `as`, link, call from C or run standalone)
Progression, each with a tiny C test harness that calls into the hand-written routine so I get pass/fail feedback without a solution key:
- A `_start` "exit with code N" program with no libc, using the `exit` syscall directly — teaches the raw ELF entry point and syscall convention before touching any C interop.
- A hand-written function following the System V AMD64 calling convention, called from C (e.g. `long add3(long,long,long)`), to internalize argument registers and the return-in-rax rule.
- A function that uses the stack explicitly (push/pop, local scratch space, respecting 16-byte alignment before `call`).
- A loop written directly in asm (e.g. sum 1..n, or strlen) with labels and conditional jumps, no C control flow to lean on.
- A function that calls another function (including calling back into a C function) — stack alignment and callee-saved register discipline.
- A raw syscall exercise beyond exit: e.g. `write(1, buf, len)` via syscall directly, no libc.
- Optional stretch: a recursive function in pure asm (e.g. factorial) to force manual stack frame management.

Each Track B exercise's `TASK.md` should state the exact function signature / syscall contract expected, since there's no compiler to catch a wrong contract — but should NOT give the instruction sequence.

## Learning philosophy to encode in the top-level README

- Optimize for reps: many short exercises, not a few long ones.
- Every exercise ends with running a real command against real output — `objdump -d`, `gdb` (`disas`, `stepi`, `info registers`), `size`, `nm`, `readelf` — not reading prose about what those tools would show.
- When a concept needs background, the README/TASK should say "read `man 2 syscalls`" / "read `info as`" / "`man 5 elf`" / "GCC docs on `-S`/`-fverbose-asm`" etc., pointing at terminal-accessible docs (`man`, `info`, `--help`, `gdb`'s `help`) over external reading.
- No generated solutions, no answer files — verification is "does it compile/assemble/link and produce the expected exit code or output," which each `Makefile`/`TASK.md` should make checkable by me (e.g. via `echo $?`, expected stdout).
- Keep each exercise self-contained (own directory, own tiny Makefile target) so I can jump around instead of working strictly in order.

## Deliverable

Write out the full directory tree and file contents now (README, Makefile(s), each exercise's `TASK.md` and starter files). Do not write any `.c`/`.s` solution bodies — only stubs, signatures, harnesses, and TODOs for me to fill in.
