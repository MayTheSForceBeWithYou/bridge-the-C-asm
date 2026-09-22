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
  linker fills holes, pulls in C runtime startup (`crt`), and produces something the kernel
  can `exec`.

### What `make` is naming for you

This exercise's Makefile pulls shared rules from `../../common.mk`. The targets are
vocabulary, not magic:

| You type | Rough equivalent | File you should open afterward |
| -------- | ---------------- | ------------------------------ |
| `make` / `make bin` | link an executable from objects | `./hello` (run it) |
| `make preprocess` | `gcc -E … -o hello.i` | `hello.i` |
| `make asm` | `gcc -S -fverbose-asm … -o hello.s` | `hello.s` |
| `make obj` | `gcc -c … -o hello.o` | `hello.o` (use `readelf`/`objdump`, not a text editor alone) |
| `make disasm` | `objdump -d hello > hello.lst` | `hello.lst` |

`make disasm` does **not** invent a new compile stage. It runs `objdump -d` on the binary
you already linked and saves the listing. That is why `hello.lst` and the live output of
`objdump -d hello` match line-for-line.

Default `CFLAGS` here include `-ggdb -Wall -Wextra -O0`. `-O0` keeps frames and calls easy
to see; later exercises change optimization on purpose.

## What "preprocessed" actually means

A `.c` file mixes two languages that look like one file: **preprocessor directives** and
**C**. The compiler proper never sees the directives. `#include <stdio.h>` means "paste the
contents of that header into this stream." `#define` macros expand into tokens before
parsing. The output is still C source — just expanded — which is why `.i` files are often
tens of thousands of lines for a handful of lines you wrote.

### How to navigate `hello.i` without reading all of it

1. Open `hello.i` in your editor.
2. Search for a unique substring of *your* message string (the text inside `printf`).
3. Notice the sea of declarations above it — prototypes, typedefs, macros from the headers
   your `#include` pulled in.
4. Near the top and around expansions you will also see lines starting with `#` that record
   file/line provenance (so compilers and debuggers can still blame the right header). Those
   are markers, not executable code.

When you change only the string inside `printf` and re-run `make preprocess`, the `.i` file
still changes in that one place (and nearby `#` markers may shift). Most of the bulk stays
identical. That is normal: the preprocessor is not optimizing; it is textual expansion.

**Rejected wrong reading:** "The preprocessor wrote thousands of lines of logic for my
hello world." Almost all of that bulk is the *declaration surface* of the C library and
headers — enough for the compiler to know what `printf` is — not a second implementation
of your program.

## What assembly is doing in `.s`

The compiler's job at `-S` is to choose instructions and data layout for your C. With
`-fverbose-asm` (this repo's default for `make asm`), GCC plants comments naming C
variables and temporaries next to instructions. Those comments are a teaching aid. They are
not part of what the CPU executes; the assembler ignores them as comments.

### What to look for in `hello.s`

Search for `main:` (a label — note the colon). Around it at `-O0` you should be able to
spot, in some order:

- Frame setup: often `pushq %rbp` then `movq %rsp, %rbp` (AT&T syntax: source first,
  destination second, registers with `%`, immediates with `$`).
- A reference to your string — sometimes a `.rodata` / `.section .rodata` label elsewhere
  in the file, plus an instruction that loads an address (`leaq …(%rip), …` is common in
  position-independent code).
- A `call` toward `printf` (the symbol may appear as `printf@PLT` or similar). In `.s` /
  `.o` that symbol is still an *unresolved external* — the linker has not yet fixed it to a
  final address.

AT&T syntax is the GNU default. Intel syntax (destination first, no `%`) shows up if you
ask for it (`objdump -M intel` or `gcc -S -masm=intel`). Same machine instructions, different
spelling. This track defaults to AT&T so your eyes match `gcc -S` and `as`.

**Rejected wrong reading:** "I can run `hello.s` like a script." The CPU never fetches `.s`.
`as` (or `gcc -c`) must turn it into machine code; the linker must still produce an
executable.

## Object file versus executable

An ELF *relocatable* object (`.o`) and an ELF *executable* share a format family, but they
answer different questions:

- **`.o`:** "Here is machine code for this translation unit, plus a list of symbols I still
  need (`printf`, and whatever the C runtime expects)."
- **executable (`hello`):** "Here is a loadable image and an entry path the OS can start"
  (`_start` → C library startup → your `main`).

### A concrete way to compare them

After `make obj` and `make` (or `make bin`):

```bash
readelf -h hello.o > readelf.o
readelf -h hello > readelf.bin
diff -u readelf.o readelf.bin
```

You are not hunting for a single magic field. You are training your eye on *which* header
fields differ: file type (relocatable vs executable / PIE), entry point presence, and later
— with `readelf -S` / `size` / `nm` — sections and symbols. Exact field names live in
`man 5 elf` and `man 1 readelf` when you need spelling; the lesson's job is to tell you
*why* those fields should differ.

`objdump -d hello` shows `main` with call targets that already point somewhere meaningful
(often a Procedure Linkage Table stub such as `printf@plt` — Track E returns to that
machinery). On `hello.o`, the same logical call may still look like a relocation placeholder.
Neither view is "more true"; they are different stages of the same story.

**Rejected wrong reading:** "Linking just renames `hello.o` to `hello`." Linking resolves
symbols, pulls in startup object code, and builds a loadable image. Size, symbol tables, and
ELF type change for a reason.

## How to read one `objdump -d` line

`make disasm` writes `objdump -d hello` into `hello.lst`. Same bytes, same text — the
`.lst` file is a saved copy of that listing so you can scroll it in an editor.

A typical line inside `<main>:` looks like this (shape from a real `hello.lst` on this
track; your left-column addresses may differ slightly when PIE/layout shifts, but the
*columns* do not):

```text
    1192:	e8 a9 fe ff ff       	call   1040 <printf@plt>
```

Read it left to right as **four different kinds of information**:

| Column | Example | What it is |
| ------ | ------- | ---------- |
| Instruction address | `1192:` | Where *this* instruction lives in the ELF image (hex). **This** is "the instruction address." |
| Raw machine bytes | `e8 a9 fe ff ff` | The bytes stored at that address. |
| Mnemonic + operands | `call   1040 <printf@plt>` | Human decoding of those bytes. |
| Symbol hint | `<printf@plt>` | `objdump` naming the target when it can. |

### The RIP-relative trap (why `0xda` and `0x2f4b` confuse people)

Before `<main>:` you will see C runtime startup, often including `_start`. Lines there can
look like:

```text
    1068:	48 8d 3d da 00 00 00 	lea    0xda(%rip),%rdi        # 1149 <main>
    106f:	ff 15 4b 2f 00 00    	call   *0x2f4b(%rip)        # 3fc0 <__libc_start_main@GLIBC_2.34>
```

Here **neither `0xda` nor `0x2f4b` is the instruction address.**

- Instruction addresses are the left column: `1068:` and `106f:`.
- `0xda` and `0x2f4b` are **offsets from the next instruction's RIP**, encoded inside the
  machine bytes. At run time the CPU adds them to RIP.
- The `# 1149 <main>` / `# 3fc0 <…>` comments are `objdump` already doing that math for
  you — those are **resolved targets**, not the addresses of the `lea` / `call` themselves.

So "note the instruction address and the target annotation" on the `lea` line means:
address `1068`, target annotation `# 1149 <main>`.

### How to find *your* `main` without drowning in CRT glue

1. In `hello.lst`, search for the label `<main>:` (angle brackets and a colon).
2. Treat everything above it (`_init`, `.plt`, `_start`, `register_tm_clones`, …) as
   runtime and library glue. You will study that glue in later tracks. For this lesson,
   your C begins at `<main>:`.
3. At `-O0`, `main` usually opens with stack setup: `push %rbp`, then `mov %rsp,%rbp`,
   often a `sub` that grows the frame. Those are not calls into libc.
4. Scroll within `main` until the mnemonic is `call` or `callq` (commonly
   `call … <printf@plt>`). On that line, left column = where the call instruction lives;
   the operand / `<…@plt>` hint = where it is trying to go.

**Rejected wrong reading:** "The first `call` in the file is my `printf`." The first
`call`-like instructions are often inside `_start` or the PLT. Anchor on `<main>:` first.

## Worked example

**The situation.** You have the starter `hello.c` that prints one line and returns 0. You
run `make preprocess asm obj disasm` and open the outputs.

**Step 1 — skim `.i` for your string.** Search for the literal text of the message. Above
it you will find oceans of header content. The rejected wrong reading is: "the preprocessor
generated all that code for my `printf`." No — almost all of it is the declaration surface
of the C library pasted in so the compiler knows what `printf` is. Your logic is still a
handful of lines near the bottom.

**Step 2 — open `.s` and find `main:`.** You should see a function label, some stack frame
setup at `-O0`, a reference toward your string, and a `call` toward `printf`. The rejected
wrong reading is: "this `.s` is what `./hello` runs." The CPU never sees `.s`; assembly and
linking still have to happen.

**Step 3 — compare `.o` to `hello` with `readelf -h` (and optionally `size`).** Save both
headers and diff them as shown earlier. Expect different type / entry-point / section
pictures. The rejected wrong reading is: "linking just renames `.o` to `hello`."

**Step 4 — in `hello.lst`, find `<main>:` and the call into libc.** Search for `<main>:`.
Ignore `_start` and the PLT until you are inside `main` — that is how you avoid mistaking
`0xda` / `0x2f4b` (RIP offsets in startup code) for instruction addresses.

Inside `main`, note the stack-setup instructions first. Then find the `call` toward
`printf@plt` (or the equivalent symbol on your listing). Record:

- the **instruction address** (left-hand hex with a colon);
- the **target annotation** (what appears after `call`).

Those two numbers answer different questions: *where is the call instruction?* versus
*where is it trying to go?*

**Step 5 — change only the message string, rebuild the pipeline, re-open `.i` / `.s` /
`.lst`.** Expect the string bytes / literal contents to change. Do not expect `main`'s
prologue shape to reinvent itself just because the greeting text changed. If the prologue
*did* change a lot, you probably also changed optimization level or compiler flags by
accident — check that you still built at `-O0`.

## Distinctions worth keeping straight

- **Preprocessor vs compiler vs assembler vs linker** — four jobs, four artifacts. Blaming
  "the compiler" for a link error wastes time.
- **`.s` text vs machine code** — editable literature versus bytes in `.o` / the executable.
- **Inspecting (`objdump`) vs building** — `.lst` is a view; deleting it does not change
  the binary. `hello.lst` and `objdump -d hello` are the same listing.
- **Instruction address vs RIP offset vs `#` target** — left column vs `0xNN(%rip)` vs the
  `# abs <symbol>` comment. Only the left column is "where this instruction lives."
- **Your `main` vs CRT / PLT** — always search `<main>:` before interpreting the first
  interesting-looking `call` in the file.
- **Changing a string** — expect `.i` / `.s` / binary *data* to reflect it; do not expect
  the *shape* of `main`'s prologue to rewrite itself for a string edit alone.

## Check yourself

Close this file and answer from memory, then verify against your artifacts:

1. Which artifact still contains `#include` expansion as C text?
2. Which artifact first contains machine code for `main`?
3. Why can a `.o` fail to run even if assembly succeeded?
4. You change only the message string and rebuild. Which artifacts must change? Which
   might look almost identical in *structure* but differ in data bytes?
5. On a `lea 0xda(%rip),… # 1149 <main>` line, which token is the instruction address,
   which is an encoded offset, and which is the resolved target?
6. In `hello.lst`, what exact search string takes you to your C rather than `_start`?

If any answer is fuzzy, re-read the matching section above — do not open `man gcc` yet.
When you need the exact flag for "stop after preprocess," *then* look up `-E`.

## Key takeaways

- The pipeline is a sequence of real files you can open; each stage has a job you can name.
- `.i` is expanded C; `.s` is compiler-chosen assembly; `.o` is unlinked machine code;
  the final binary is linked ELF; `.lst` is only a disassembly view.
- Hex on an `objdump` line is not all the same kind of number — learn the columns.
- Man pages document flags and formats. They do not teach you what to look for — that is
  what this lesson was for.

## Lookup (not the lesson)

Use these when you already know the question:

- `man 1 gcc` — flags `-E`, `-S`, `-c`, `-o`
- `man 1 objdump` — `-d`, `-h`
- `man 1 readelf` — `-h`, `-S`
- `man 1 size` / `man 1 nm` — footprint and symbols, when you want another angle on `.o` vs binary
- `man 5 elf` — ELF structure names
- `info gcc` — longer narrative if you want it after the practice

Now open `TASK.md` and do the practice.
