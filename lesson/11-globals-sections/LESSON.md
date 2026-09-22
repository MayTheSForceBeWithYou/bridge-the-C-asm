# 11 — Globals and sections

Globals and statics are not stack slots. They are symbols living in ELF
*sections*: typically `.data` for initialized mutable data, `.bss` for zero /
uninitialized mutable data, `.rodata` for const data, and `.text` for code.
Tools like `nm`, `size`, `objdump -h`, and `readelf -S` let you see that map
without guessing. By the end of this lesson you should place a modified global,
a BSS global, a `const`, and a `static` into the right buckets and read `nm`'s
type letters for them.

## What this lesson asks of you

Define several storage-duration examples in one program, build it, and run the
inspectors listed in `TASK.md`. This lesson teaches what the letters and section
names *mean*; man pages supply spelling for flags.

## What `make` / tools are naming for you

| You type | What you get | Open / read |
| -------- | ------------ | ----------- |
| `make` | linked `prog` | run it if it prints |
| `nm prog` | symbol table with type letters | stdout |
| `size prog` | text / data / bss totals | stdout |
| `objdump -h prog` | section headers (sizes, VMAs) | stdout or redirect |
| `readelf -S prog` | section headers (ELF view) | stdout |
| `make asm` | `prog.s` | how code *refers* to globals (RIP-relative) |

`make disasm` still only saves `objdump -d` — useful for `.text`, not a substitute
for `nm` / `objdump -h` when you care about data sections.

## Section roles (practical)

| Section | Typical contents | Example C |
| ------- | ---------------- | --------- |
| `.text` | machine code for functions | `int f(void){…}` |
| `.data` | mutable objects with nonzero initializers | `int g = 1;` |
| `.bss` | mutable objects that start as zero; file may omit zero payload | `int h;` or often `int z = 0;` |
| `.rodata` | const-qualified data and typically string literals | `const int c = 3;` |

### How to read one `nm` line

```text
0000000000004028 D g
0000000000004030 B h
0000000000004018 d s
0000000000002004 R c
0000000000001149 T main
```

(Addresses vary wildly under PIE/ASLR layouts and link order — read the *letter*
and the *name*.)

| Field | Example | Meaning |
| ----- | ------- | ------- |
| Value / address | `0000000000004028` | Where the symbol lives in the image (nm's column). |
| Type letter | `D`, `B`, `R`, `T`, `d`… | Kind of symbol — see table below. |
| Name | `g` | Linker-visible name (C `static` may be local / decorated). |

Common letters you will meet:

| Letter | Usually means |
| ------ | ------------- |
| `T` / `t` | text (code) — global / local |
| `D` / `d` | data (initialized mutable) |
| `B` / `b` | bss (zeroed mutable) |
| `R` / `r` | read-only data |
| Uppercase | typically global binding |
| Lowercase | typically local (`static`) |

**Rejected wrong reading:** "`static` means the variable is on the stack because
it is inside a function." Local `static` is still persistent data, not an
automatic. You should *not* see it as `-N(%rbp)` for its storage — you may see
RIP-relative access to a `.data`/`.bss` symbol instead.

## `static` changes visibility, not "stackness"

A `static` file-scope variable still has static storage duration — it lives for
the whole program — but its linkage is internal. `nm` usually shows it as a
lowercase letter. A `static` *local* inside a function also has static storage
duration (one persistent object), not automatic stack allocation, even though its
*name* is scoped to the function.

### Navigation for a local `static`

1. In C, find `static int once` inside the function.
2. In `nm prog`, find a symbol that corresponds (may look like `once.0` or similar).
3. Note its letter (`b`/`d`/`d`…) — not `t`.
4. In `prog.s`, search for that symbol or for stores that are *not* `-N(%rbp)`
   inside the function that increments it.

## Why `.bss` exists as a separate idea

If every zero-initialized global were stored as literal zero bytes in the file,
executables would bloat for no reason. ELF records the size of `.bss` and the
loader supplies zeroed pages at runtime. That is why `size` reports a bss number
even when `objdump -s -j .bss` may show little on-disk content.

**Rejected wrong reading:** "`.bss` is empty so `h` does not exist." `.bss`
reserves zeroed memory at load time; the on-disk section can look sparse.

### How to read `size` and `objdump -h`

```text
   text    data     bss     dec     hex filename
   1234     560      16    1810     712 prog
```

| Column | Meaning |
| ------ | ------- |
| text | roughly code |
| data | initialized data contribution |
| bss | zeroed data size at runtime |

`objdump -h` lines look like:

```text
Idx Name          Size      VMA               LMA               File off  Algn
 12 .data         00000010  0000000000004000  …                 …         2**3
 13 .bss          00000008  0000000000004010  …                 …         2**3
```

| Field | Use |
| ----- | --- |
| Name | `.data`, `.bss`, `.rodata`, `.text` |
| Size | how big |
| Algn | section alignment power-of-two |

You are training recognition, not memorizing every VMA.

## Worked example

**The situation.** You define `int g = 1;`, `int h;`, `static int s = 2;`,
`const int c = 3;`, and a `static int once` inside a function that increments it.

**Step 1 — `nm prog`.** Find each symbol and letter. Map `g`→`D`/`d`, `h`→`B`/`b`,
`c`→`R`/`r`, `main`→`T`, file-scope `s`→ lowercase data letter, local `static`
similarly local.

**Step 2 — `objdump -h` / `readelf -S`.** Match sections to the story.

**Step 3 — `size`.** Relate the numeric buckets to what you added. Growing
`.data` versus `.bss` differently when you add initializers is the tell.

**Step 4 — tweak one initializer.** Change `g`'s initializer or make `h`
explicitly `= 0` and see whether it moves between bss and data — compilers may
still put explicit zero in bss; observe rather than assume.


## RIP-relative access to globals in `.s`

Code rarely uses absolute addresses for `g` on PIE builds. Expect:

```text
        movl    $1, g(%rip)
        movl    g(%rip), %eax
```

| Form | Meaning |
| ---- | ------- |
| `g(%rip)` | address of symbol `g` via RIP-relative relocation |
| Not `-N(%rbp)` | this is *not* an automatic local |

That is a quick asm-side check that your "global" is not accidentally an
automatic. Pair it with `nm`'s letter for the full story.

## Distinctions worth keeping straight

- **`.data` vs `.bss`** — nonzero init vs zero / uninitialized.
- **`static` linkage vs static storage** — visibility versus lifetime.
- **`nm` letter vs section name** — two views of one symbol.
- **Locals automatic vs locals `static`** — stack slot versus persistent object.
- **`objdump -d` vs `objdump -h` / `nm`** — code listing vs section/symbol map.

## Check yourself

1. Where should `int g = 1;` usually appear: data or bss?
2. What does a lowercase `b` from `nm` often tell you?
3. Why is an uninitialized global not typically stored as a huge pile of zero
   bytes in the file?
4. Does `static int counter;` inside `f` live at an offset from `%rbp`?
5. Which tool gives a one-line text/data/bss summary?
6. Why is `make disasm` alone insufficient for this lesson's competence?

## Key takeaways

- Globals/statics map to ELF sections you can list with binutils.
- `nm` letters are a quick key; sections are the ground truth layout.
- `static` on a local changes lifetime/linkage, not "make it automatic."
- Inspect with `nm` / `size` / `objdump -h` / `readelf -S` — not only `objdump -d`.

## Lookup (not the lesson)

- `man 1 nm` — type letters
- `man 1 size`, `man 1 objdump`, `man 1 readelf`
- `man 5 elf` — section concepts

Now open `TASK.md` and do the practice.
