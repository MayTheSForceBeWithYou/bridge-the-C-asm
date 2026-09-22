# 15 — Function pointers

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

## What `make` is naming for you

| You type | Open | Search |
| -------- | ---- | ------ |
| `make asm` | `prog.s` | `apply:`, `call *`, `inc(%rip)` |
| `make O=2 asm` | copy first | Did `apply` survive? Direct `call inc`? |
| `make disasm` | `prog.lst` | `<apply>:`, look for `callq *%…` |

## Loading and calling — decode the lines

At `-O0`, `apply` typically receives the function pointer in an argument
register (same integer ABI as lesson 04 — addresses are integers of pointer
width). The body moves that pointer into a register and executes `call *%reg`
(AT&T syntax for an indirect call).

```text
apply:
        pushq   %rbp
        movq    %rsp, %rbp
        movq    %rdi, -8(%rbp)    # fp
        movl    %esi, -12(%rbp)   # x
        movq    -8(%rbp), %rax    # load fp into %rax
        movl    -12(%rbp), %edi   # arg for the callee
        call    *%rax             # indirect call through fp
        …
```

| Line | Meaning |
| ---- | ------- |
| `movq %rdi, -8(%rbp)` | FP arrived in `%rdi` (1st integer/pointer arg). |
| `movl %esi, -12(%rbp)` | `x` arrived in `%esi` (2nd arg). |
| `call *%rax` | Call *whatever address* `%rax` holds — `inc` or `dec`. |

In `main`, taking the address of `inc` may appear as:

```text
        leaq    inc(%rip), %rax
        …                       # move into %rdi for apply's first arg
        call    apply
```

### Navigation

1. Search `apply:` — find `call *`.
2. Search `main:` — find `leaq inc(%rip)` / `leaq dec(%rip)` and the `call apply`.
3. Confirm `inc` / `dec` themselves still look like ordinary functions (prologue,
   return in `%eax`).

**Rejected wrong reading:** "`call *%rax` means 'call the absolute address 0'
somehow." The asterisk marks indirection through the register's *value*, which
should hold `inc` or `dec`'s address.

**Rejected wrong reading:** "passing `inc` without `&` in C means no address is
involved." Function designators decay to pointers; the asm still moves an
address.

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

| Form | Kind |
| ---- | ---- |
| `call printf@PLT` | direct call to a PLT stub (linker/loader fills the real target later) |
| `call *%rax` | indirect call through a value you computed / received |
| `call inc` | direct call to a known symbol |

## Worked example

**The situation.** `apply(inc, 41)` and `apply(dec, 41)` should produce 42 and
40. You inspect both optimization levels.

**Step 1 — `-O0` `apply`.** Find the indirect `call *`. Note which register
holds the pointer at the call.

**Step 2 — `-O0` `main`.** See how addresses of `inc`/`dec` are materialized and
passed.

**Step 3 — `-O2`.** Check whether `apply` survives and whether calls became
direct. Record the factual outcome with citations.

**Step 4 — relate to ABI.** Arguments to `inc` still use the normal register;
the *callee identity* was what was indirect, not the integer ABI itself. Return
still in `%eax`.


## Objdump indirect call spelling

In `prog.lst` you may see:

```text
    1184:       ff d0                   callq  *%rax
```

| Token | Meaning |
| ----- | ------- |
| `ff d0` | encoding for `call *%rax` |
| `*%rax` | target = contents of `%rax` |

Compare to `callq 1040 <printf@plt>` — fixed relative displacement to a PLT
stub, not a register indirection.


## Keeping indirection alive at `-O2` (optional)

If `-O2` erases `apply` completely and you still want to *see* `call *`, you can
complicate the pointer (for example choose `inc` vs `dec` from an input the
compiler cannot constant-fold). Only do that after you have already recorded the
honest `-O2` outcome on the simple program — the simple outcome is part of the
lesson.


## Return register unchanged by indirection

Whether you `call inc` or `call *%rax` where `%rax` holds `&inc`, the `int`
result still returns in `%eax`. Indirection changes *who* runs, not the integer
return channel from lesson 05.


## Rejected wrong reading (syntax)

"**I must write `apply(&inc, x)` or no pointer is formed.**" In C, `inc` in that
position decays to a pointer. Writing `&inc` is also fine. Either way the asm
moves an address into `apply`'s first argument register.

## Distinctions worth keeping straight

- **Direct `call symbol` vs indirect `call *%reg`** — fixed vs computed target.
- **Function address vs function result** — pointer versus `int` return.
- **PLT stub vs your function pointer** — different indirection stories.
- **`-O0` pedagogical indirection vs `-O2` devirtualization** — both valid.
- **C decay rules vs asm address moves** — source sugar, machine addresses.

## Check yourself

1. What AT&T syntax marks an indirect call through a register?
2. Which lesson's registers carry the function pointer into `apply`?
3. Why might `-O2` show no indirect call even though your C uses `fp(x)`?
4. Does an indirect call change where `inc` returns its `int`?
5. How do you tell `call *%rax` from `call printf@PLT` in a listing?
6. What search finds the pointer materialization in `main`?

## Key takeaways

- Function pointers are addresses; calls through them are indirect (`call *%reg`).
- `-O0` usually shows the indirection; `-O2` may restore direct calls.
- Passing `inc` still means passing an address at the machine level.
- Read both optimization levels before concluding "this is how pointers always
  look."

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
