# 19 — Inline asm bridge

Extended Asm lets you drop a few assembler instructions inside C while still
letting GCC allocate registers and keep the surrounding function ABI-correct.
You write operands with constraints (letters like `r` for a general register,
`=` for output), and the compiler stitches your fragment into the larger
prologue/epilogue story from lesson 02. By the end of this lesson you should
implement a tiny `add2_asm`, explain your constraint choices, and know that
`asm volatile` affects *how the compiler treats the statement*, not magic CPU
volatility.

## What this lesson asks of you

Implement `long add2_asm(long a, long b)` with Extended Asm that adds the
operands and produces a C return value; keep `add2_c` as a pure-C twin; print
both. Skim `info gcc` Extended Asm *after* you understand the goal here — Lookup
is reference, not the teacher. Practice steps live in `TASK.md`.

## What `make` is naming for you

| You type | Open | Look for |
| -------- | ---- | -------- |
| `make asm` | `prog.s` | your `addq` fragment inside `add2_asm`, plus prologue |
| `make O=2 asm` | copy first | reordering vs a nearby global store |
| `make disasm` | `prog.lst` | `<add2_asm>:` still has a normal function wrapper |

## Operand anatomy (concept first)

An Extended Asm statement names:

| Part | Role | Example hint |
| ---- | ---- | ------------ |
| **Outputs** | C lvalues the asm will write; marked with `=` in the constraint | `"=r"(sum)` |
| **Inputs** | C expressions read by the asm | `"r"(a)`, `"r"(b)` |
| **Clobbers** | registers or `"memory"` your asm destroys beyond the operands | `"cc"` if you care about flags |

Constraint letters tell GCC *what kind of place* to pick. For a first add, a
register constraint (`r`) for inputs and a separate output register is the usual
pattern you will discover while iterating — match what compiles and what the
generated `.s` shows around your fragment.

Your fragment should assume AT&T syntax inside GNU as / GCC's asm templates
(`%0`, `%1` placeholders expand to the operands GCC chose):

```c
__asm__ ("addq %2, %0"
         : "=r"(sum)           /* %0 output */
         : "0"(a), "r"(b));    /* %0 also input a; %2 is b — exact form may vary */
```

(Exact constraint spelling is your experiment — Lookup `info gcc` for letter
tables once you know you need a letter.)

### How to read the generated `.s` around your fragment

```text
add2_asm:
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -8(%rbp)     # still a normal ABI entry
        movq    %rsi, -16(%rbp)
        …
        movq    …, %rax
        movq    …, %rdx
        addq    %rdx, %rax         # your template, registers filled in
        …
        popq    %rbp
        ret                        # return still via ABI (%rax)
```

| Observation | Meaning |
| ----------- | ------- |
| Prologue still present at `-O0` | Inline asm did not replace the function wrapper |
| Your `addq` uses concrete regs | `%0`/`%1` were assigned by GCC |
| Result ends in `%rax` before `ret` | C `return sum;` uses the integer return channel |

**Rejected wrong reading:** "inline asm replaces the need for a C function
prologue." GCC still generates a normal function wrapper unless it inlines
everything.

**Rejected wrong reading:** "constraint `r` means `%rax` specifically." `r`
means *some* GPR; GCC picks.

## `volatile` on asm

`asm volatile` (or `__asm__ __volatile__`) tells GCC not to delete the asm even
if outputs look unused, and restricts some reordering relative to other
volatile accesses. It does **not** automatically freeze all surrounding memory
traffic.

### Reordering experiment (recognition)

1. Declare a global `int g`.
2. In `add2_asm` or nearby in `main`, store to `g` just before/after the asm.
3. Build `make O=2 asm` and see whether the store and the asm keep source order.
4. If they flip and you needed a barrier, a `"memory"` clobber is the usual next
   lever — only after you understand why.

**Rejected wrong reading:** "`asm volatile` means the CPU treats memory like
`volatile int`." Different mechanisms (lesson 18 vs this statement attribute).

## Worked example

**The situation.** `add2_asm(2, 3)` and `add2_c(2, 3)` both print `5`. You inspect
`-O0` and `-O2` asm around the inline fragment.

**Step 1 — get a compiling version.** Focus on one output operand feeding the
`return`.

**Step 2 — read the generated `.s`.** See which registers `%0`/`%1` became and
how the result lands in the return register.

**Step 3 — compare to `add2_c`.** Same ABI; different body authorship.

**Step 4 — `-O2` reordering experiment.** Place a store to a global near the asm;
observe; adjust only if you understand why.


## Minimal constraint experiment log

Keep a tiny log while iterating:

```text
attempt 1: constraints ________  compile? Y/N  .s shows regs ________
attempt 2: …
final: output ________ inputs ________ clobbers ________
```

The log is part of learning — Extended Asm is iterative. Once it compiles, the
`.s` is the authority for which registers `%0` became.


## Return path reminder

Even if your asm template writes directly into a register that happens to be
`%rax`, prefer thinking: "output operand → C variable → `return` → ABI." That
survives register allocation changes at `-O2` better than hard-coding "my asm
must use `%rax`."


## Compare to lesson 02 deliberately

Open your `-O0` `add2_asm` and point at `push %rbp` / `mov %rsp,%rbp` / `ret`.
Those bookends are still lesson 02's story; your `addq` is only the payload.
Saying that out loud is part of finishing this bridge lesson.

## Distinctions worth keeping straight

- **Template string vs final registers** — placeholders versus GCC's choices.
- **Output constraint vs return ABI** — you produce an operand; C `return` still
  uses the ABI channel.
- **`asm volatile` vs `volatile` object** — statement semantics vs object access
  rules (lesson 18).
- **Teaching fragment vs production asm** — keep the first example tiny.
- **`"memory"` clobber vs hoping `volatile` alone orders everything** — know which
  lever you used.

## Check yourself

1. What does an `=` in a constraint signify?
2. Why might GCC still emit `push %rbp` around your two-instruction add?
3. Does `asm volatile` alone guarantee order against every nearby non-volatile
   store?
4. Where should you look up constraint letter spellings once you know you need
   them?
5. If your asm writes `sum` and you `return sum;`, who places the value in
   `%rax` — your template or the compiler's return lowering?
6. What search finds your fragment: the mnemonic you wrote (`addq`) inside
   `add2_asm`?

## Key takeaways

- Extended Asm binds C operands to asm placeholders with constraints.
- GCC still owns the function's ABI wrapper and register allocation.
- `asm volatile` limits deletion/reordering; it is not omniscient.
- Learn the mechanism here; use `info gcc` as Lookup for letter tables.

## Lookup (not the lesson)

- `info gcc` — Extended Asm (after this lesson)
- `man 1 gcc`, `man 1 objdump`

Now open `TASK.md` and do the practice.
