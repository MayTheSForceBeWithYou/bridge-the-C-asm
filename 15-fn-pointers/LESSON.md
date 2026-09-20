# 15 — Function pointers
<!-- concepts: c.p4.fnptr.call -->

A function pointer holds the address of executable code. Calling through it is an
*indirect* call: the target is in a register (or memory) rather than hard-wired
into the `call` immediate. At `-O0` you will usually see that indirection
clearly. At `-O2`, inlining and constant propagation may turn `apply(inc, x)`
back into a direct `call inc` — or eliminate `apply` entirely. By the end of this
lesson you should recognize `call *%reg` and know that optimization can erase the
indirection you came to study.

## What this lesson asks of you

Define `inc` and `dec`, a helper `apply(int (*fp)(int), int x)`, and call `apply`
with both pointers from `main`. Compare `-O0` and `-O2` asm/disasm. Practice in
`TASK.md`.

## Loading and calling

At `-O0`, `apply` typically receives the function pointer in an argument
register (same integer ABI as lesson 04 — addresses are integers of pointer
width). The body moves that pointer into a register and executes `call *%reg`
(AT&T syntax for an indirect call). The pointed-to function still uses normal
prologue/epilogue and return-register conventions.

In `main`, taking the address of `inc` may appear as `leaq inc(%rip), %…` (PIE)
before the call to `apply`.

## Optimization surprises

If the compiler can see that `fp` is always `inc` at a given call site, it may:

- Inline `apply` and emit `call inc` directly, or
- Inline `inc` itself into the caller.

Then your carefully written indirection disappears — correctly. To study
indirect calls, trust `-O0` first, or complicate the pointer enough that it is
not a compile-time constant (without turning the exercise into opacity for its
own sake).

## Indirect call and the PLT

When you call a *named* libc function, you often see `call printf@PLT` — still a
direct-looking relocation into the Procedure Linkage Table. A function pointer
call is different: the target address is already in a register. Do not confuse
PLT indirection (dynamic linking machinery) with `call *%reg` (your pointer).

## Worked example

**The situation.** `apply(inc, 41)` and `apply(dec, 41)` should produce 42 and
40. You inspect both optimization levels.

**Step 1 — `-O0` `apply`.** Find the indirect `call *`. The rejected wrong
reading is: "`call *%rax` means 'call the absolute address 0' somehow." The
asterisk marks indirection through the register's value, which should hold
`inc` or `dec`'s address.

**Step 2 — `-O0` `main`.** See how addresses of `inc`/`dec` are materialized and
passed. The rejected wrong reading is: "passing `inc` without `&` in C means no
address is involved." Function designators decay to pointers; the asm still
moves an address.

**Step 3 — `-O2`.** Check whether `apply` survives and whether calls became
direct. Record the factual outcome.

**Step 4 — relate to ABI.** Arguments to `inc` still use the normal register;
the *callee identity* was what was indirect, not the integer ABI itself.

## Distinctions worth keeping straight

- **Direct `call symbol` vs indirect `call *%reg`** — fixed vs computed target.
- **Function address vs function result** — pointer versus `int` return.
- **`-O0` pedagogical indirection vs `-O2` devirtualization** — both valid.
- **C decay rules vs asm address moves** — source sugar, machine addresses.

## Check yourself

1. What AT&T syntax marks an indirect call through a register?
2. Which lesson's registers carry the function pointer into `apply`?
3. Why might `-O2` show no indirect call even though your C uses `fp(x)`?
4. Does an indirect call change where `inc` returns its `int`?

## Key takeaways

- Function pointers are addresses; calls through them are indirect.
- `-O0` usually shows `call *%reg`; `-O2` may restore direct calls.
- Passing `inc` still means passing an address at the machine level.
- Read both optimization levels before concluding "this is how pointers always
  look."

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
