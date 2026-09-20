# 12 — String literals in `.rodata`
<!-- concepts: c.p2.strings.nul, c.p5.elf.rodata -->

A string literal in C source is typically an array of `char` bytes living in
read-only data, referenced by address from code. Identical literals may be
merged; a mutable `char buf[] = "...";` is a different beast — it copies bytes
into writable storage. By the end of this lesson you should find literal bytes
with `objdump -s -j .rodata` / `readelf -x`, see how `printf` references them
(often RIP-relative on PIE builds), and contrast that with a mutable buffer
initializer.

## What this lesson asks of you

Write a small program with several literals (some duplicated), mix `const char *`
and `char []`, then inspect asm and rodata dumps. `TASK.md` lists commands.

## How code refers to bytes

On modern x86-64 PIE (position-independent executable) builds, expect
RIP-relative addressing: something like `leaq .LC0(%rip), %rdi` before a
`printf` call. Absolute 32-bit addresses are less common in default PIE
binaries. The label (`.LC0`, …) is the compiler's name for that literal pool
entry.

`objdump -s -j .rodata` shows hex + ASCII so you can *see* the characters,
including the terminating `NUL`.

## Merging and mutability

Compilers often merge identical string literals (same contents) into one pool
entry. That is an optimization with observable consequences if you ever tried to
write through a `char *` that secretly points at rodata — undefined behavior in
C, and often a segmentation fault in practice.

`char buf[] = "hi";` allocates a mutable array (commonly on the stack for
automatics, or in `.data` for static duration) and *initializes* it from a
literal. You may still see the initializer bytes in rodata used as a copy
source, plus writable storage for `buf` itself. That dual presence surprises
people who expected only one copy of `"hi"` in the whole binary.

## PIE reminder

Default modern GCC builds produce position-independent executables. Literal
addresses therefore tend to be computed relative to `%rip` so the whole image
can slide in memory (ASLR). If you ever build with `-no-pie` for contrast, the
addressing mode in the `.s` may change — another reason to read *this* box's
output rather than a textbook screenshot.

## Worked example

**The situation.** You call `printf` twice with the exact same literal text, once
with a different text, and you also have `char buf[] = "mut";` you modify.

**Step 1 — dump `.rodata`.** Find your strings. The rejected wrong reading is:
"every string I typed appears twice because I called `printf` twice." Calls
reference addresses; they do not duplicate pool bytes per call site by default.

**Step 2 — check merging.** See whether two identical literals share one label /
one byte sequence. The rejected wrong reading is: "merged strings mean my
program printed once." Merging is storage, not control flow.

**Step 3 — inspect the `printf` call site in `.s`.** Note RIP-relative versus
other forms. Record what you actually got on this box.

**Step 4 — inspect `buf`.** Find writable storage and any copy setup. Confirm
that modifying `buf[0]` does not require writing `.rodata`.

## Distinctions worth keeping straight

- **Literal in rodata vs mutable `char []`** — permission and lifetime differ.
- **Pool merging vs call count** — references ≠ copies.
- **RIP-relative vs absolute** — PIE defaults matter.
- **Section dump vs disassembly** — bytes versus instructions that point at them.

## Check yourself

1. Which tool view shows the ASCII bytes of a literal most directly?
2. What addressing mode should you expect for a literal address on a typical PIE
   build?
3. Why can writing through a pointer to a string literal crash?
4. How does `char buf[] = "...";` differ from `const char *p = "...";` in storage?

## Key takeaways

- String literals usually live in `.rodata` and are referenced by address.
- Identical literals may merge; mutable arrays copy into writable memory.
- PIE code often uses RIP-relative loads of literal addresses.
- Confirm with `gcc -S` plus `objdump -s` / `readelf -x`.

## Lookup (not the lesson)

- `man 1 objdump` — `-s`, `-j`, `-d`
- `man 1 readelf` — `-x`
- `man 5 elf`, `man 1 gcc`

Now open `TASK.md` and do the practice.
