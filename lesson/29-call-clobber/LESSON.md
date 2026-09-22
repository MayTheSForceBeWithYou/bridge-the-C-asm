# 29 — Call clobbers

A `call` does not only transfer control — from the caller's point of view it may
destroy the contents of some registers. Those are *caller-saved* (clobbered)
registers: if you still need their values after the call, you must save them
yourself (or let the compiler reload them). Other registers are *callee-saved*:
if the callee uses them, it must restore them before returning. By the end of
this lesson you should derive both sets from *your* compiler output or gdb
session on this SysV AMD64 box — not from recalling an ABI PDF.

## What this lesson asks of you

Arrange a situation where distinctive values sit in several registers, execute a
`call`, and see which values survive. Two workable approaches: (1) inspect `-O0`
asm around a call and note which registers the caller reloads afterward; (2) use
gdb `info registers` before and after the `call`. Practice details are in
`TASK.md`.

## What `make` / gdb are naming for you

| You type | Evidence you get |
| -------- | ---------------- |
| `make asm` at `-O0` | pre-call spills + post-call reloads in `main` |
| `make disasm` | same in `prog.lst` — search `<main>:` then the `call` |
| gdb `break *0x…` / break on call site | live register compare |
| Optional callee `.s` | `push %rbx` / `push %r12`… in prologue → callee-saved hints |

## How to read caller saves in asm

At `-O0`, GCC is often generous with spills: it may store values to stack slots
before `call` and reload afterward. Those reloads are evidence that the compiler
treats the corresponding registers as unsafe across the call.

### Shape to hunt

```text
main:
        …
        movl    $0x1111, %eax
        movl    %eax, -20(%rbp)     # spill — keep 0x1111 live across call
        movl    $0x2222, %ebx       # may use callee-saved %rbx
        …
        call    helper              # or printf@PLT
        movl    -20(%rbp), %eax     # reload — suggests %eax was clobbered
        …
        # use %ebx without reload — suggests %rbx survived (callee-saved)
```

| Pattern after `call` | Likely classification for that register |
| -------------------- | --------------------------------------- |
| Reload from stack into `%reg` before use | caller-saved / clobbered — compiler did not trust it |
| Use `%reg` with no intervening reload, value still correct | callee-saved *or* value was dead (force liveness!) |
| Callee `push %reg` / `pop %reg` around its body | callee preserves that reg because it is callee-saved |

### Navigation

1. Put distinctive constants (`0x1111`, `0x2222`, …) in locals that you *print
   after* the call so they stay live.
2. Search `main:` → find `call`.
3. List stores above the call and loads below it; note which hard registers
   appear.
4. Optionally open the callee and list prologue `push`es of non-`%rbp` regs.

**Rejected wrong reading:** "if GCC spilled `%rbx`, then `%rbx` is caller-saved."
Spilling a callee-saved register can also happen if the *callee* needs it —
focus on the *caller's* need to reload after *its* call because the ABI says the
call may smash that reg. `%rbx` surviving across `printf` without reload is the
cleaner callee-saved clue.

**Rejected wrong reading:** "whatever the callee pushes is caller-saved."
Push-on-entry is how a callee *preserves* callee-saved regs.

## gdb confirmation

Break on the `call` instruction (use `disassemble main` to get the address, then
`break *0x…`), note registers with `info registers`, `stepi` over the call (or
break on the return site), compare again.

| Step | Command shape | What you record |
| ---- | ------------- | --------------- |
| Find call site | `disassemble main` | address of `call` |
| Break on it | `break *0xADDRESS` | |
| Snapshot | `info registers` | rax rcx rdx rsi rdi r8 r9 r10 r11 rbx r12… |
| Step over call | `stepi` until past `call` | |
| Snapshot again | `info registers` | which changed |

Any register that changed without your explicit wish is effectively clobbered
from the caller's perspective (the callee or libc used it).

Ordinary `printf` is a heavy caller-saved storm — useful and realistic.

## Overlap with argument registers

Several argument registers are also caller-saved. That is intentional: after you
place args and execute `call`, you should assume those arg registers may be
garbage unless the callee's return value (or a callee-saved reg you know was
preserved) carries what you need. Lesson 04 taught where args start; this lesson
teaches what survives the call.

| Also typically clobbered | Why it bites juniors |
| ------------------------ | -------------------- |
| `%rax` | return register — of course it changes |
| `%rdi`/`%rsi`/… arg regs | still holding args is not promised after return |
| `%r10`/`%r11` | scratch; easy to forget |

## Worked example

**The situation.** `main` computes several live values, calls a helper (or
`printf`), then uses those values again in a way the optimizer cannot delete at
`-O0`.

**Step 1 — list registers reloaded after `call` in the `.s`.** Those are strong
hints for caller-saved.

**Step 2 — list registers the callee pushes in its prologue.** Often callee-saved
ones it plans to use.

**Step 3 — cross-check with gdb** if the listing is ambiguous.

**Step 4 — write two short lists** (clobbered vs must-preserve) from evidence —
not from a PDF.


## Starter sets to verify (from evidence only)

After your experiments, you should be able to fill:

```text
Likely caller-saved (clobbered):  ______________________________
Likely callee-saved (preserved): ______________________________
```

Common SysV outcomes students rediscover: arg regs + `%rax` + `%r10`/`%r11`
among clobbered; `%rbx`, `%rbp`, `%r12`–`%r15` among preserved. **Still cite your
listing/gdb** — the point is the method, not copying this paragraph into your
notes as gospel without evidence.


## Using `printf` as the storm

Calling `printf` is enough: it is a real ABI callee that clobbers generously.
You do not need a handwritten trash function for the first pass. Keep your
distinctive locals live by printing them *after* `printf` returns — that forces
reloads you can see in the `.s`.


## Two-list deliverable

Your notes should end with two bullet lists titled **Caller-saved (from
evidence)** and **Callee-saved (from evidence)**, each with at least three
registers and a one-line citation (`reload after printf`, `callee push/pop`,
`gdb changed`, …).

## Distinctions worth keeping straight

- **Caller-saved vs callee-saved** — who is responsible for preserving.
- **Spill for safety vs spill for register pressure** — read the why.
- **`call` clobber set vs your function's locals** — ABI versus implementation.
- **`%rax` as return vs `%rax` as clobbered scratch** — both true after many calls.
- **Discovery vs PDF memory** — this track requires discovery.

## Check yourself

1. If `main` reloads `%rax` after `printf`, what does that suggest about `%rax`?
2. If a callee `push %rbx` / later `pop %rbx`, what is `%rbx` likely classified as?
3. Why must you keep values live after the call in your experiment?
4. Why is there no ABI man page in the verified Lookup set for this?
5. How do you set a breakpoint on a specific `call` instruction in gdb?
6. Why is "I still see my constant in `%rdi` after `call`" an unreliable proof
   that `%rdi` is callee-saved?

## Key takeaways

- Calls may smash caller-saved registers; callers reload if needed.
- Callees must restore callee-saved registers they use.
- Derive both sets from asm/gdb on this platform with distinctive live values.
- Arg registers and `%rax` are easy clobber traps after `call`.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, `man 1 gdb`
- gdb: `help info registers`, `help disassemble`

Now open `TASK.md` and do the practice.
