# 29 — Call clobbers
<!-- concepts: c.p5.abi.clobber -->

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

## How to read caller saves in asm

At `-O0`, GCC is often generous with spills: it may store values to stack slots
before `call` and reload afterward. Those reloads are evidence that the compiler
treats the corresponding registers as unsafe across the call. Conversely,
registers that keep live values with no save/restore may be in the callee-saved
set — or the value may simply be dead. Use distinctive constants and keep values
live (print them after the call) so deadness does not confuse you.

A carefully written callee that deliberately trashes many registers (in asm or
via volatile tricks) can make clobbers obvious, but even ordinary `printf` is a
heavy caller-saved storm — useful and realistic.

## gdb confirmation

Break on the `call` instruction, note registers, `stepi` over the call (or break
on return), compare again. Any register that changed without your explicit wish
is effectively clobbered from the caller's perspective (the callee or libc used
it).

## Overlap with argument registers

Several argument registers are also caller-saved. That is intentional: after you
place args and execute `call`, you should assume those arg registers may be
garbage unless the callee's return value (or a callee-saved reg you know was
preserved) carries what you need. Lesson 04 taught where args start; this lesson
teaches what survives the call.

## Worked example

**The situation.** `main` computes several live values, calls a helper (or
`printf`), then uses those values again in a way the optimizer cannot delete at
`-O0`.

**Step 1 — list registers reloaded after `call` in the `.s`.** Those are strong
hints for caller-saved. The rejected wrong reading is: "if GCC spilled `%rbx`,
then `%rbx` is caller-saved." Spilling a callee-saved register can also happen
if the *callee* needs it — focus on the *caller's* need to reload after *its*
call because the ABI says the call may smash that reg.

**Step 2 — list registers the callee pushes in its prologue.** Often callee-saved
ones it plans to use. The rejected wrong reading is: "whatever the callee
pushes is caller-saved." Push-on-entry is how a callee *preserves* callee-saved
regs.

**Step 3 — cross-check with gdb** if the listing is ambiguous.

**Step 4 — write two short lists** (clobbered vs must-preserve) from evidence.

## Distinctions worth keeping straight

- **Caller-saved vs callee-saved** — who is responsible for preserving.
- **Spill for safety vs spill for register pressure** — read the why.
- **`call` clobber set vs your function's locals** — ABI versus implementation.
- **Discovery vs PDF memory** — this track requires discovery.

## Check yourself

1. If `main` reloads `%rax` after `printf`, what does that suggest about `%rax`?
2. If a callee `push %rbx` / later `pop %rbx`, what is `%rbx` likely classified as?
3. Why must you keep values live after the call in your experiment?
4. Why is there no ABI man page in the verified Lookup set for this?

## Key takeaways

- Calls may smash caller-saved registers; callers reload if needed.
- Callees must restore callee-saved registers they use.
- Derive both sets from asm/gdb on this platform.
- Distinctive live values make the evidence readable.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, `man 1 gdb`
- gdb: `help info registers`

Now open `TASK.md` and do the practice.
