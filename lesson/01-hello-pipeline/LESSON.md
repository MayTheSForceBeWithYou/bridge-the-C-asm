# 01 — Hello pipeline

A C source file is not what the CPU runs. Between `hello.c` and a running process sits a
pipeline of real artifacts — preprocessed text, assembly, an object file, and a linked
ELF binary — each one a different *kind* of truth about your program. By the end of this
lesson you should be able to name what each artifact is for, predict which one will change
when you edit a string literal, and find `main` in a disassembly without guessing.

## What this lesson asks of you

Given a tiny C program, drive the translation pipeline yourself and *read* each output.
The competence is not "I can type `make`." It is: when something about the final binary
surprises you later in this track, you already know which stage invented that surprise.

`TASK.md` in this directory is the practice. This file is the teaching. Man and info pages
are for looking up a flag spelling once you know *what* you are looking for — they are not
a substitute for this lesson.

## The four artifacts (and the binary)

Start from source and walk forward. Each step takes the previous artifact as input.

| Stage | Typical command | Output | What it is |
| ----- | --------------- | ------ | ---------- |
| Preprocess | `gcc -E` | `.i` | C after `#include` / `#define` expansion — still text, often huge |
| Compile | `gcc -S` | `.s` | Assembly language your compiler chose (AT&T syntax with GNU tools here) |
| Assemble | `gcc -c` | `.o` | Machine code + metadata for *this* translation unit — not yet a program |
| Link | `gcc` (driver runs `ld`) | `hello` (no suffix) | A complete ELF executable: your `.o` plus startup and libraries |
| Inspect | `objdump -d` | `.lst` (here) | A *human-readable view* of machine code already in an object or binary |

Two distinctions that save hours later:

- **`.s` is not the binary.** It is text the assembler still has to turn into bytes. You can
  edit `.s` by hand (Track B); you cannot execute it directly.
- **`.o` is not the program.** It has unresolved references (`printf` is still a hole). The
  linker fills holes, pulls in `crt` startup, and produces something the kernel can `exec`.

The Makefile targets `preprocess`, `asm`, `obj`, `bin`, and `disasm` are just named handles
for those stages. You could type the `gcc` / `objdump` lines yourself; the names exist so
every exercise in this repo shares one vocabulary.

## What "preprocessed" actually means

A `.c` file is full of directives the compiler proper never sees. `#include <stdio.h>` means
"paste the contents of that header here." `#define` macros expand into tokens. The result
is still C source — just expanded — which is why `.i` files are often tens of thousands of
lines for a five-line `main`.

When you change only the string inside `printf`, the `.i` file still changes in that one
place (and the `#` line markers may shift). Most of the bulk is unchanged header text. That
is normal: the preprocessor is not "optimizing"; it is textual expansion.

## What assembly is doing in `.s`

The compiler's job at `-S` is to choose instructions and data layout for your C. With
`-fverbose-asm` (this repo's default for `make asm`), GCC plants comments naming C
variables and temporaries next to instructions. Those comments are a teaching aid, not part
of the language the CPU executes.

String literals usually land in a read-only data section (often shown with `.section
.rodata` or similar). `main` appears as a label. A call to `printf` appears as a `call`
(or equivalent) to an external symbol — still not resolved to an address in `.s` / `.o`.

AT&T syntax (GNU default) writes sources then destination (`mov %rdi, %rax` style). You will
see Intel syntax later with `objdump -M intel`; same machine, different spelling. This track
defaults to AT&T so your eyes match `gcc -S` and `as`.

## Object file versus executable

An ELF *relocatable* object (`.o`) and an ELF *executable* share a format family (`man 5
elf` is the reference page for field names) but answer different questions:

- **`.o`:** "Here is machine code for this `.c`, and a list of symbols I still need."
- **executable:** "Here is a memory image layout and an entry path the loader can start."

That is why `readelf -h` / `objdump -h` on both is worth thirty seconds: section counts,
whether an entry point exists, and whether dynamic linking info appears are all visible
without reading every byte.

`objdump -d` on the final `hello` shows `main` with real call targets (often via the PLT
for libc — Track E returns here). On `.o`, the same call may look like a relocation stub.
Neither view is "more true"; they are different stages of the same story.

## Worked example

**The situation.** You have the starter `hello.c` that prints one line and returns 0. You
run `make preprocess asm obj disasm` and open the four outputs.

**Step 1 — skim `.i` for your string.** Search for the literal text of the message. Above
it you will find oceans of header content. The rejected wrong reading is: "the preprocessor
generated all that code for my `printf`." No — almost all of it is the *declaration surface*
of the C library pasted in so the compiler knows what `printf` is. Your logic is still a
handful of lines at the bottom.

**Step 2 — open `.s` and find `main`.** You should see a function label, some stack frame
setup at `-O0`, a load or reference of the string, and a `call` toward `printf`. The
rejected wrong reading is: "this `.s` is what `./hello` runs." The CPU never sees `.s`;
`as` still has to assemble it, and `ld` still has to link it.

**Step 3 — compare `.o` to `hello` with `readelf -h` or `size`.** Expect different type /
section / symbol pictures. The rejected wrong reading is: "linking just renames `.o` to
`hello`." Linking resolves symbols and stitches startup code; file size and symbol tables
change for a reason.

**Step 4 — in `hello.lst`, find `main` and the call into libc.** Note the instruction
address and the target annotation `objdump` prints. You do not need to memorize opcodes
yet; you need to recognize "this is the call" versus "this is stack setup."

## Distinctions worth keeping straight

- **Preprocessor vs compiler vs assembler vs linker** — four jobs, four artifacts. Blaming
  "the compiler" for a link error wastes time.
- **`.s` text vs machine code** — editable literature versus bytes.
- **Inspecting (`objdump`) vs building** — `.lst` is a view; deleting it does not change
  the binary.
- **Changing a string** — expect `.i` / `.s` / binary contents to reflect it; do not expect
  the *shape* of `main`'s prologue to rewrite itself for a string edit alone.

## Check yourself

Close this file and answer from memory, then verify against your artifacts:

1. Which artifact still contains `#include` expansion as C text?
2. Which artifact first contains machine code for `main`?
3. Why can a `.o` fail to run even if assembly succeeded?
4. You change only the message string and rebuild. Which artifacts must change? Which
   might look almost identical in *structure* but differ in data bytes?

If any answer is fuzzy, re-read the matching section above — do not open `man gcc` yet.
When you need the exact flag for "stop after preprocess," *then* look up `-E`.

## Key takeaways

- The pipeline is a sequence of real files you can open; each stage has a job you can name.
- `.i` is expanded C; `.s` is compiler-chosen assembly; `.o` is unlinked machine code;
  the final binary is linked ELF.
- `objdump` / `readelf` are how you *look*; they are not additional compile stages.
- Man pages document flags and formats. They do not teach you what to look for — that is
  what this lesson was for.

## Lookup (not the lesson)

Use these when you already know the question:

- `man 1 gcc` — flags `-E`, `-S`, `-c`, `-o`
- `man 1 objdump` — `-d`, `-h`
- `man 1 readelf` — `-h`, `-S`
- `man 5 elf` — ELF structure names
- `info gcc` — longer narrative if you want it after the practice

Now open `TASK.md` and do the practice.