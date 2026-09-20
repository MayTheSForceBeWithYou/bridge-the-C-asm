# 11 — Globals and sections
<!-- concepts: c.p5.elf.sections -->

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

## Section roles (practical)

- **`.text`** — machine code for functions.
- **`.data`** — mutable objects with nonzero initializers (for example `int g = 1;`).
- **`.bss`** — mutable objects that should start as zero; the file may omit huge
  zero payloads and record size instead (`int h;` at file scope).
- **`.rodata`** — const-qualified data and typically string literals (lesson 12).

`size` summarizes text / data / bss totals. `objdump -h` / `readelf -S` list
sections. `nm` lists symbols with a type letter: common ones you will meet
include `T`/`t` (text), `D`/`d` (data), `B`/`b` (bss), `R`/`r` (read-only data).
Uppercase often means global visibility; lowercase often means local (`static`).

## `static` changes visibility, not "stackness"

A `static` file-scope variable still has static storage duration — it lives for
the whole program — but its linkage is internal. `nm` usually shows it as a
lowercase letter and a name that may be decorated. A `static` *local* inside a
function also has static storage duration (one persistent object), not automatic
stack allocation, even though its *name* is scoped to the function.

## Why `.bss` exists as a separate idea

If every zero-initialized global were stored as literal zero bytes in the file,
executables would bloat for no reason. ELF records the size of `.bss` and the
loader supplies zeroed pages at runtime. That is why `size` reports a bss number
even when `objdump -s -j .bss` may show little on-disk content.

## Worked example

**The situation.** You define `int g = 1;`, `int h;`, `static int s = 2;`,
`const int c = 3;`, and a `static int once` inside a function that increments it.

**Step 1 — `nm prog`.** Find each symbol and letter. The rejected wrong reading
is: "`static` means the variable is on the stack because it is inside a
function." Local `static` is still persistent data, not an automatic.

**Step 2 — `objdump -h` / `readelf -S`.** Match sections to the story. The
rejected wrong reading is: "`.bss` is empty so `h` does not exist." `.bss`
reserves zeroed memory at load time; the on-disk section can look sparse.

**Step 3 — `size`.** Relate the numeric buckets to what you added. Growing `.data`
versus `.bss` differently when you add initializers is the tell.

**Step 4 — tweak one initializer.** Change `g`'s initializer or make `h`
explicitly `= 0` and see whether it moves between bss and data — compilers may
still put explicit zero in bss; observe rather than assume.

## Distinctions worth keeping straight

- **`.data` vs `.bss`** — nonzero init vs zero / uninitialized.
- **`static` linkage vs static storage** — visibility versus lifetime.
- **`nm` letter vs section name** — two views of one symbol.
- **Locals automatic vs locals `static`** — stack slot versus persistent object.

## Check yourself

1. Where should `int g = 1;` usually appear: data or bss?
2. What does a lowercase `b` from `nm` often tell you?
3. Why is an uninitialized global not typically stored as a huge pile of zero
   bytes in the file?
4. Does `static int counter;` inside `f` live at an offset from `%rbp`?

## Key takeaways

- Globals/statics map to ELF sections you can list with binutils.
- `nm` letters are a quick key; sections are the ground truth layout.
- `static` on a local changes lifetime/linkage, not "make it automatic."
- Inspect with `nm` / `size` / `objdump -h` / `readelf -S`.

## Lookup (not the lesson)

- `man 1 nm` — type letters
- `man 1 size`, `man 1 objdump`, `man 1 readelf`
- `man 5 elf` — section concepts

Now open `TASK.md` and do the practice.
