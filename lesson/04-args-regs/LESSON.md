# 04 — Argument registers

When one C function calls another, arguments have to live somewhere the callee
can find them. On x86-64 System V AMD64 (GNU/Linux), the first six *integer /
pointer* arguments travel in fixed registers; further arguments spill to the
stack. There is no ABI man page in the verified reference set for this track —
you discover the convention by reading what `gcc -S` emits. By the end of this
lesson you should name those six registers from evidence and know where a
seventh argument and a scalar return value appear.

## What this lesson asks of you

Implement tiny functions of one, two, six, and seven `long` parameters, call them
from `main` with distinctive constants, and read the assembly. Do not look up an
ABI PDF. The listing is the teacher; Lookup entries are only for tool flags.

## What `make` is naming for you

| You type | File to open |
| -------- | ------------ |
| `make asm` | `prog.s` — call sites in `main`, callees' entry spills |
| `make disasm` | `prog.lst` — same story after link; search `<f6>:` / `<main>:` |

## How to read a call site

In `main` at `-O0`, GCC typically materializes each constant into a register (or
stack slot) and then places arguments where the callee expects them *just before*
`call`. Distinct constants (`1`, `2`, `3`, …) make it obvious which argument
landed where.

A real `-O0` call to `f6(1,2,3,4,5,6)` looks like this shape:

```text
        movl    $6, %r9d
        movl    $5, %r8d
        movl    $4, %ecx
        movl    $3, %edx
        movl    $2, %esi
        movl    $1, %edi
        call    f6
```

| Constant | Register loaded | Logical argument |
| -------- | --------------- | ---------------- |
| `$1` | `%edi` | 1st |
| `$2` | `%esi` | 2nd |
| `$3` | `%edx` | 3rd |
| `$4` | `%ecx` | 4th |
| `$5` | `%r8d` | 5th |
| `$6` | `%r9d` | 6th |

(Widths may show as `%rdi` / `%edi` etc.; for small positive constants GCC often
uses 32-bit forms that zero-extend into the 64-bit register.)

**Rejected wrong reading:** "whatever register is mentioned first in the file is
argument one." Argument identity comes from *which constant* maps to *which C
parameter*, not from textual order of unrelated instructions earlier in `main`.

### Navigation move

1. Search `main:` in `prog.s`.
2. Search within `main` for `call f6` (then `call f7`).
3. Read *upward* a handful of lines from that `call` — those moves are the
   argument setup.
4. Build your table from constants → registers before you look at the callee.

## The six-register pattern (caller and callee)

Build `f1` through `f6` first. For each arity, note which registers are loaded
at the call site and which registers the callee reads on entry. You should see a
stable assignment for arguments 1–6. Write it down from *your* `.s` file. That
list *is* your ABI note for the rest of Track A.

At `-O0`, the callee often spills immediately:

```text
f6:
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -8(%rbp)   # a
        movq    %rsi, -16(%rbp)  # b
        movq    %rdx, -24(%rbp)  # c
        movq    %rcx, -32(%rbp)  # d
        movq    %r8, -40(%rbp)   # e
        movq    %r9, -48(%rbp)   # f
        …
```

| What you see | What it means |
| ------------ | ------------- |
| `movq %rdi, -8(%rbp)` | Incoming arg 1 was in `%rdi`; `-O0` copied it to a local slot. |
| Later loads from `-8(%rbp)` | Body uses the spill, not a different ABI. |

**Rejected wrong reading:** "the callee picks any registers it likes for incoming
args." Incoming locations are fixed by convention; after that, temps are free.
Spilling to `-N(%rbp)` does not redefine the ABI — it is pedagogical `-O0`
behavior.

## The seventh argument lives on the stack

When you add `f7`, six registers will still be used for the first six arguments;
the seventh will be passed in memory. Look for a store relative to `%rsp` before
the `call` — often a literal `pushq $7` or a `movq $7, (%rsp)`-style setup — and
a matching load from a **positive** stack offset in the callee (commonly
`16(%rbp)` after a standard prologue: saved `%rbp` at `0(%rbp)`, return address
at `8(%rbp)`, first stack arg at `16(%rbp)`).

Example caller fragment shape:

```text
        pushq   $7              # 7th arg on stack
        movl    $6, %r9d
        …                       # 1..5 into the usual regs
        call    f7
        addq    $8, %rsp        # pop the stack arg
```

Example callee read of `g`:

```text
        movq    16(%rbp), %rax  # g
```

**Rejected wrong reading:** "the seventh argument also gets a dedicated register
on this ABI." If your listing shows stack traffic for `g`, believe the listing.

## Return path (preview of lesson 05)

After `call` returns, scalar integer results show up where the *caller* consumes
them — almost always by reading `%rax` / `%eax`. In the `f6`+`f7` sum example you
may see `addl %ebx, %eax` or similar: `%eax` still holds a return value from the
latest `call`.

## Worked example

**The situation.** You implement `long f6(long a,b,c,d,e,f)` returning a sum, and
`long f7(..., long g)` likewise. `main` calls both with constants `1…6` and
`1…7`.

**Step 1 — inspect `main`'s call to `f6`.** List the six moves into registers.
Order them by argument position using the constant values.

**Step 2 — inspect `f6`'s body.** Confirm it reads the same registers (or their
spills). Match `%rdi`↔`a`, `%rsi`↔`b`, … using verbose comments.

**Step 3 — inspect `f7`.** Find the seventh constant on the stack at the call
site and the matching load in `f7` (often `16(%rbp)`).

**Step 4 — note the return path.** Where does `main` take the sum from after
`call`? Remember that location for lesson 05.


## Stack diagram for the seventh argument

After a standard prologue in `f7`:

```text
16(%rbp)  →  first stack argument (g)
 8(%rbp)  →  return address
 0(%rbp)  →  saved caller's %rbp
-8(%rbp)  →  spilled a (from %rdi), etc.
```

That is why `movq 16(%rbp), %rax` is the usual `-O0` read of argument 7 — not
because "16 is magic," but because two qwords sit between `%rbp` and the first
stack arg.

### Objdump call-site shape

In `prog.lst`, search `<main>:` then the `call` toward `f6`. Argument setup still
appears as moves into `%edi`…`%r9d` above that line. Left column = instruction
address of each move; do not confuse those addresses with the *values* `$1`…`$6`
being placed into registers.

## Distinctions worth keeping straight

- **Argument registers vs return register** — incoming versus outgoing.
- **First six in regs vs seventh on stack** — arity changes the picture.
- **ABI home vs `-O0` spill** — spilling to a local does not redefine the ABI.
- **`%edi` vs `%rdi`** — 32-bit form vs full 64-bit name; same logical arg slot.
- **Discovering vs memorizing from a PDF** — this track requires discovery.

## Check yourself

1. How would you experimentally find which register carries argument 3?
2. Where should you look for argument 7 in the caller before `call`?
3. After `f6` returns a `long`, which register should you inspect first in the
   caller?
4. Why might `f6` at `-O0` store `%rdi` to `-8(%rbp)` even though `%rdi` already
   held the argument?
5. In AT&T, does `movl $1, %edi` place argument 1 or argument something else —
   and how do you know?
6. What search string finds the call site setup without reading all of CRT?

## Key takeaways

- SysV AMD64 passes the first six integer/pointer args in fixed registers —
  derive the map from distinctive constants at the call site.
- Further args pass on the stack; your `.s` shows the offsets (often `16(%rbp)`
  for the first stack arg in the callee).
- Scalar integer returns are consumed from `%rax` / `%eax`.
- `-O0` spills are implementation texture, not a different calling convention.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- No ABI man page is in the verified set — use compiler output.

Now open `TASK.md` and do the practice.
