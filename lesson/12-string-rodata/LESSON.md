# 12 — String literals in `.rodata`

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

## What `make` / tools are naming for you

| You type | Output | Open |
| -------- | ------ | ---- |
| `make asm` | `prog.s` | `.LC0` labels + `leaq .LC0(%rip), %rdi` |
| `make` / `make bin` | `prog` | needed before section dumps |
| `make disasm` | `prog.lst` | call sites; still search `<main>:` first |
| `objdump -s -j .rodata prog` | hex + ASCII dump of `.rodata` | stdout |
| `readelf -x .rodata prog` | another hex dump view | stdout |

`make disasm` does **not** dump `.rodata` bytes — it only saves `objdump -d`.
You need `-s -j .rodata` (or `readelf -x`) for the character payload.

## How to read an `objdump -s` rodata dump

```text
Contents of section .rodata:
 2004 68656c6c 6f000000 68690000 6d757400  hello...hi..mut.
```

| Piece | Meaning |
| ----- | ------- |
| Left hex (e.g. `2004`) | offset/VMA-ish column for the dump — not an instruction address in `.text` |
| Groups of hex bytes | the literal contents including `00` NULs |
| ASCII gutter | printable view; `.` for non-printable (including NUL) |

Search the ASCII gutter for your string. Confirm a `00` byte terminates it.

**Rejected wrong reading:** "every string I typed appears twice because I called
`printf` twice." Calls reference addresses; they do not duplicate pool bytes per
call site by default.

## How code refers to bytes

On modern x86-64 PIE builds, expect RIP-relative addressing:

```text
        leaq    .LC0(%rip), %rdi
        call    printf@PLT
```

| Token | Meaning |
| ----- | ------- |
| `.LC0` | compiler's label for that literal pool entry |
| `(%rip)` | address = next_RIP + displacement (PIE-friendly) |
| `%rdi` | first integer/pointer arg — the `const char *` to `printf` |

In `prog.lst`, the same idea may look like `lea 0x…(%rip),%rdi` with a `#`
annotation naming the symbol — remember lesson 01: the left column is the
instruction address; the `0x…(%rip)` is an offset; the `#` comment is the
resolved target.

### Navigation

1. Open `prog.s`; search `.LC` or a unique substring in a `.string` / `.ascii`
   directive.
2. Search `main:` / `printf` call sites for `leaq …(%rip)`.
3. Dump `.rodata` and match bytes to those labels' contents.
4. Check whether two identical C literals share one `.LC*` label (merging).

## Merging and mutability

Compilers often merge identical string literals into one pool entry. That is an
optimization with observable consequences if you ever tried to write through a
`char *` that secretly points at rodata — undefined behavior in C, and often a
segmentation fault in practice.

`char buf[] = "hi";` allocates a mutable array (commonly on the stack for
automatics, or in `.data` for static duration) and *initializes* it from a
literal. You may still see the initializer bytes in rodata used as a copy
source, plus writable storage for `buf` itself. That dual presence surprises
people who expected only one copy of `"hi"` in the whole binary.

**Rejected wrong reading:** "merged strings mean my program printed once."
Merging is storage, not control flow.

**Rejected wrong reading:** "modifying `buf[0]` writes `.rodata`." If `buf` is a
mutable array, you write the writable copy — that is the point of the contrast
with `const char *p = "…"`.

## PIE reminder

Default modern GCC builds produce position-independent executables. Literal
addresses therefore tend to be computed relative to `%rip` so the whole image
can slide in memory (ASLR). If you ever build with `-no-pie` for contrast, the
addressing mode in the `.s` may change — another reason to read *this* box's
output rather than a textbook screenshot.

## Worked example

**The situation.** You call `printf` twice with the exact same literal text, once
with a different text, and you also have `char buf[] = "mut";` you modify.

**Step 1 — dump `.rodata`.** Find your strings in the ASCII gutter.

**Step 2 — check merging.** See whether two identical literals share one label /
one byte sequence.

**Step 3 — inspect the `printf` call site in `.s`.** Note RIP-relative versus
other forms. Record what you actually got on this box.

**Step 4 — inspect `buf`.** Find writable storage and any copy setup. Confirm
that modifying `buf[0]` does not require writing `.rodata`.


## Matching `.LC0` in `.s` to dump bytes

In `prog.s` you may see:

```text
.LC0:
        .string "hello"
```

The `.string` directive implies a trailing NUL. In the `objdump -s` dump, find
`68 65 6c 6c 6f 00` (`hello\0`). If two call sites both load `.LC0(%rip)`, they
share one pool entry — merging succeeded.

### Mutable buffer dual presence

For `char buf[] = "mut";` on the stack, expect:

1. Bytes `"mut"` somewhere in `.rodata` (copy source), and
2. Stack stores / a block copy into `-N(%rbp)` for `buf`.

Writes touch (2), not (1). If you only dump `.rodata`, you will still see `mut`
— that does not prove `buf` lives there permanently.


## `printf` and varargs (light awareness)

Loading the format string into `%rdi` is the pointer story of this lesson. Extra
integer args for `printf` use the integer ABI; floating args would use XMMs
(lesson 16). Do not let varargs setup distract you from finding `.LC0(%rip)`.

### Checking identity of merged literals

In gdb or with `printf("%p\n", (void*)"same");` twice, identical literal
addresses suggest merging (implementation-defined territory — treat as
observation, not a portable guarantee). The `.rodata` dump + single `.LC*` label
is the cleaner evidence for this track.

## Distinctions worth keeping straight

- **Literal in rodata vs mutable `char []`** — permission and lifetime differ.
- **Pool merging vs call count** — references ≠ copies.
- **RIP-relative vs absolute** — PIE defaults matter.
- **Section dump (`-s`) vs disassembly (`-d`)** — bytes versus instructions that
  point at them.
- **Instruction address vs `(%rip)` offset** — lesson 01 columns still apply.

## Check yourself

1. Which tool view shows the ASCII bytes of a literal most directly?
2. What addressing mode should you expect for a literal address on a typical PIE
   build?
3. Why can writing through a pointer to a string literal crash?
4. How does `char buf[] = "...";` differ from `const char *p = "...";` in storage?
5. Does `make disasm` alone show you the characters of the string?
6. How do you tell a merged literal from "printf was only called once"?

## Key takeaways

- String literals usually live in `.rodata` and are referenced by address.
- Identical literals may merge; mutable arrays copy into writable memory.
- PIE code often uses RIP-relative loads of literal addresses.
- Confirm with `gcc -S` plus `objdump -s` / `readelf -x` — not only `objdump -d`.

## Lookup (not the lesson)

- `man 1 objdump` — `-s`, `-j`, `-d`
- `man 1 readelf` — `-x`
- `man 5 elf`, `man 1 gcc`

Now open `TASK.md` and do the practice.
