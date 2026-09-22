# 02 — Prologue / epilogue

Every C function you write becomes a labeled region of machine code with an entry
and an exit. At `-O0`, GCC almost always wraps that region in a *prologue* that
sets up a frame pointer and an *epilogue* that tears it down before `ret`. By the
end of this lesson you should recognize those bookends on sight, say what
`%rbp` / `%rsp` are doing during them, and predict how `-O2` can shrink or erase
an empty function entirely.

## What this lesson asks of you

You will read compiler-generated assembly for a nearly empty function and for
`main`, at two optimization levels. The competence is not memorizing opcodes; it
is knowing which instructions are "housekeeping" so later lessons can ignore them
and focus on the interesting middle. Practice lives in `TASK.md`. Man pages are
for flag spellings once you already know what you need.

## What `make` is naming for you

This exercise's Makefile pulls shared rules from `../../common.mk` (same pattern
as lesson 01). The targets you will use:

| You type | Rough equivalent | File you should open afterward |
| -------- | ---------------- | ------------------------------ |
| `make` / `make bin` | link the executable | `./prog` (or whatever `NAME` is) |
| `make asm` | `gcc -S -fverbose-asm … -o prog.s` at default `-O0` | `prog.s` |
| `make O=2 asm` | same stop-after-compile, but `-O2` | `prog.s` (overwrites — copy aside first) |
| `make disasm` | `objdump -d prog > prog.lst` | `prog.lst` |
| `make O=2 disasm` | rebuild at `-O2`, then disassemble | `prog.lst` (again: copy if you need both) |

`make O=2 asm` does **not** invent a second filename. The `O=` knob rewrites
`CFLAGS` and then `asm` still writes `$(NAME).s`. If you want both listings, copy
`prog.s` to something like `prog-O0.s` before the optimized rebuild.

## Frame pointer setup at `-O0`

On x86-64 System V (the ABI GNU/Linux uses here), a typical unoptimized function
opens like this (shape from a real `gcc -O0 -fverbose-asm -S` on this track;
addresses in `.lst` will differ under PIE, but the *mnemonics* match):

```text
empty:
        pushq   %rbp
        movq    %rsp, %rbp
        nop
        popq    %rbp
        ret
```

Read that as a story, not a blob:

| Instruction | What it does to the machine |
| ----------- | --------------------------- |
| `pushq %rbp` | Writes the caller's `%rbp` to `(%rsp-8)`, then sets `%rsp -= 8`. Saves the old frame pointer. |
| `movq %rsp, %rbp` | Copies the *current* stack pointer into `%rbp`. From now on, locals are addressed as `-N(%rbp)`. |
| `nop` (sometimes) | Placeholder; empty bodies still need *something* between prologue and epilogue at `-O0`. |
| `popq %rbp` | Restores the caller's frame pointer and grows `%rsp` by 8. |
| `ret` | Pops the return address into RIP and transfers control back to the caller. |

Optional third prologue step when the function needs stack space: `subq $N, %rsp`
reserves `N` bytes for locals / alignment. The matching close may be
`leave` (which is `mov %rbp,%rsp` then `pop %rbp`) or an explicit
`addq`/`mov` + `pop` pair.

With `-fverbose-asm` (this track's default for `make asm`), GCC plants comments
naming C entities next to instructions. Use those comments to confirm that the
`push` / `mov` pair is frame setup, not "your" logic.

### How to find the bookends without drowning

1. Open `prog.s` (or `prog.lst` after `make disasm`).
2. Search for the label `empty:` in `.s`, or `<empty>:` in `.lst`.
3. The first real instructions under that label are the prologue; the last before
   the next function label are the epilogue + `ret`.
4. Ignore `.cfi_*` lines — those are Call Frame Information for unwinders /
   debuggers, not CPU instructions. The assembler turns them into metadata, not
   opcodes.

**Rejected wrong reading:** "`.cfi_startproc` is part of the prologue the CPU
runs." It is not. If you are counting instructions the CPU executes, skip every
line that starts with `.cfi_`.

## What "empty" still costs

A C function `void empty(void) {}` has no statements, but at `-O0` it is still a
real call target: prologue, epilogue, `ret`. Calling it from `main` still emits a
`call empty`. The rejected wrong reading people make here is "empty means zero
bytes." Empty means zero *payload*; the ABI still wants a callable symbol unless
the optimizer proves the call is useless.

`main` usually looks heavier: it may align the stack (SysV wants `%rsp` 16-byte
aligned before a `call`), set up locals, and arrange a return value in `%eax`.
Compare `empty` to `main` side by side so you separate "frame ritual" from
"program work."

### Reading the call site in `main`

Search for `main:` then look for `call empty` (or `callq empty` in some
listings). A typical `-O0` shape:

```text
main:
        pushq   %rbp
        movq    %rsp, %rbp
        call    empty
        …          # return 0 setup into %eax
        popq    %rbp
        ret
```

| Token | Meaning |
| ----- | ------- |
| `call empty` | Push return address, jump to `empty`. |
| Moves into `%eax` after the call | `main`'s own return value — not `empty`'s (void returns nothing). |

**Rejected wrong reading:** "the `sub $N,%rsp` in `main` is the size of `empty`."
Frame size in `main` is about `main`'s own needs (alignment, locals), not the
callee's body length.

## What `-O2` is allowed to do

Optimization can:

- **Inline** a tiny callee into its caller, so the `call` / `ret` pair vanishes.
- **Eliminate** a function that is never needed after inlining, so the symbol may
  disappear from the `.s` / binary you inspect.
- **Omit the frame pointer** (`-fomit-frame-pointer` is common at `-O2`), addressing
  locals from `%rsp` instead of `%rbp`, which shortens or removes the classic
  `push %rbp` / `mov %rsp,%rbp` pair.
- **Shrink an empty function to a single `ret`**, or erase the call entirely when
  the callee has no observable effect.

On a real GCC build of `void empty(void) {}` called once from `main`, `-O2` may
look like:

```text
empty:
        ret

main:
        xorl    %eax, %eax
        ret
```

Here `main` never `call`s `empty` — the call was deleted as useless. `empty`
may still exist as a lonely `ret` if something else could take its address, or
it may vanish from the listing entirely. Both outcomes are legal optimizations.

So when you rebuild with `make O=2 asm`, do not expect the `-O0` shape. Expect a
different contract: the compiler still implements your C semantics, but the
housekeeping may be gone.

**Rejected wrong reading:** "`-O2` failed because my empty function vanished."
Vanishing can be correct optimization when nothing observable remains.

## How to read the same story in `objdump -d`

`make disasm` writes `objdump -d` into `prog.lst`. Search for `<empty>:` (angle
brackets + colon). A line still has the four-field shape from lesson 01:

```text
    1149:       55                      push   %rbp
    114a:       48 89 e5                mov    %rsp,%rbp
```

| Column | Example | What it is |
| ------ | ------- | ---------- |
| Instruction address | `1149:` | Where this instruction lives (hex). |
| Raw bytes | `55` | Machine encoding. |
| Mnemonic + operands | `push   %rbp` | Human decoding. |

Your left-column addresses will differ under PIE; column *meaning* does not.

**Rejected wrong reading:** "The first `push %rbp` in the whole `.lst` is my
`empty`." CRT startup and other functions also use frame pointers. Anchor on
`<empty>:` or `<main>:` first — same navigation move as lesson 01.

## Worked example

**The situation.** You implement `void empty(void) {}`, call it once from `main`,
and emit assembly at `-O0` and `-O2`.

**Step 1 — open the `-O0` `.s` and find `empty:`.** You should see a label, a
short prologue (`pushq %rbp` / `movq %rsp, %rbp`), and an epilogue ending in
`ret`. Skip `.cfi_*`. The rejected wrong reading is: "`push %rbp` is pushing my
function's return value." No — it saves the *caller's* `%rbp`. Your return value
(if any) would be in a return register at `ret` time; `empty` returns void, so
there is nothing to hand back.

**Step 2 — find `main`'s call site.** Search `call empty`. Note any stack
alignment around the call. Confirm `empty` is a real symbol, not inlined yet.
The rejected wrong reading is: "the `sub $N,%rsp` in `main` is the size of
`empty`."

**Step 3 — copy `prog.s` aside, then `make O=2 asm`.** Check whether `empty`
still appears as a standalone function, whether `main` still contains a `call`,
and whether frame pointer instructions remain. The rejected wrong reading is:
"`-O2` failed because my empty function vanished."

**Step 4 — `make disasm` / `make O=2 disasm`.** Confirm the same story in
`objdump -d` output: search `<empty>:` and `<main>:`, then compare call targets
and whether optimized builds still have a distinct `empty` symbol.


## `%rbp` vs `%rsp` during the call

Keep a short timeline for one `call empty` from `main` at `-O0`:

1. **Before `call`:** `%rsp` is 16-byte aligned (SysV). `%rbp` still belongs to `main`.
2. **`call` executes:** pushes 8-byte return address; `%rsp` drops by 8.
3. **`empty` prologue:** `push %rbp` saves `main`'s frame pointer; `mov %rsp,%rbp`
   makes `empty`'s frame; optional `sub` for locals.
4. **`empty` epilogue + `ret`:** restores `%rbp`, pops return address into RIP;
   back in `main` with `%rsp` where it was before the `call`.

If you ever see `%rbp` equal to `%rsp` right after `mov %rsp,%rbp`, that is
expected — they start equal; later `push`/`sub` make `%rsp` lower while `%rbp`
stays put so `-N(%rbp)` stays stable.

### `leave` as compressed epilogue

Some functions end with `leave` then `ret`. `leave` means: `mov %rbp, %rsp`
then `pop %rbp`. Same effect as the long form; recognize either spelling.

## Distinctions worth keeping straight

- **Prologue/epilogue vs your statements** — bookends versus payload.
- **`%rbp` frame pointer vs `%rsp` stack pointer** — stable local base versus
  moving top of stack.
- **`.cfi_*` metadata vs CPU instructions** — skip CFI when reading what runs.
- **`-O0` pedagogy shape vs `-O2` real shape** — both are valid; they answer
  different questions.
- **Inlining vs deletion** — inlining copies the body into the caller;
  elimination drops a symbol that nothing needs.
- **`make O=2 asm` overwrites `prog.s`** — copy aside if you need both.

## Check yourself

Close this file and answer from memory, then verify against your artifacts:

1. What two instructions typically open an `-O0` function that uses a frame
   pointer, and what does each accomplish?
2. Why does `void empty(void) {}` still generate machine code at `-O0`?
3. Name two ways `-O2` might change what you see for `empty` and its call site.
4. If `%rsp` changes during a function, why did classical frames bother setting
   `%rbp` at entry?
5. In `prog.s`, which lines do you skip when counting instructions the CPU runs?
6. What exact search string takes you to `empty` in `prog.lst` versus `prog.s`?

If an answer is fuzzy, re-read the matching section — do not open `man gcc` yet.
When you need the exact spelling of `-fverbose-asm` or `-O2`, *then* look it up.

## Key takeaways

- At `-O0`, expect a visible prologue/epilogue around almost every function.
- `push %rbp` / `mov %rsp,%rbp` establish a frame; epilogue undoes it before `ret`.
- Optimization may omit frame pointers, inline callees, or delete dead functions.
- Anchor on the function label before interpreting the first `push` in a file.
- You discover the shape from `gcc -S` / `objdump -d`, not from an ABI PDF.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- `info gcc` — optimization overview after practice

Now open `TASK.md` and do the practice.
