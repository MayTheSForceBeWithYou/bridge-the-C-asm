# 33 — Live ABI in hand-written asm

You already wrote (or will write) `add3` in assembly: three `long` arguments in
`%rdi`/`%rsi`/`%rdx`, sum in `%rax`. Compiling the harness proves the contract
when the test passes. Debugging proves it *while* the instructions run — before
you overwrite an argument register, and after the sum lands in `%rax`. That gap
between "tests green" and "I saw the ABI" is what this lesson closes.

## What this lesson asks of you

Launch the Makefile-linked `./add3_test`, stop at the C call site, step into
your `.s`, and read registers the way a reviewer would. The competence is
joining Track B's static ABI story to a live stop inside code *you* authored,
not only inside GCC's output.

`TASK.md` is practice. `NVIM_DAP.md` spells keys; this lesson teaches what to
look for when you press them.

## Why break on the call site first

A breakpoint on the asm symbol `add3` works when the debugger resolves the
symbol. A breakpoint on the line in `harness.c` that **calls** `add3` is often
clearer: you still have C names `a`,`b`,`c` in Scopes, you see the call about to
fire, then **F11** transfers into the callee.

After the `call`, the CPU has pushed the return address and jumped to `add3`.
Argument registers should still hold the three longs until your first
instructions clobber them. Photograph that window early. If your first
instruction is `movq %rdi, %rax` (or similar), one step later `%rdi` may still
be intact — or not, depending on what you wrote. Entry is the reliable moment.

## Reading registers in dap-ui

CodeLLDB exposes register groups under Scopes. You want the general-purpose
set: confirm `%rdi`,`%rsi`,`%rdx` match the values you saw for `a`,`b`,`c` at
the call site (or via `<leader>de`). Step with F10/F11 and watch `%rax` become
the sum. Prefer Scopes and hover before diving into the DAP REPL (`<leader>dr`);
REPL is optional power, not the first tool.

Honest limit: F10/F11 are usually **source/line** steps. A one-line asm
function may be a handful of instructions; F11 might appear to "jump" through
them depending on debug info for the `.s`. If you need every `addq` visible,
gdb `stepi` is the precise tool — same skill, tighter grain. Lesson 35 will
depend on that honesty for `%rsp`.

## Launch existing, not "Build current file"

`add3_test` is harness `.c` plus `add3.s` linked by `make`. nvim-dap's "Build
current file and launch" would `gcc -g` only the open buffer and miss your
assembly. Always `make` first, then **"Launch existing executable"** →
`./add3_test` (the `NAME` from the Makefile). Single-file throwaways can use
Build current file; this directory is not one of those.

## Worked example

**The situation.** `expect` calls `add3(1, 2, 3)` wanting `6`. You break on that
call, launch, stop in the harness.

**Step 1 — caller view.** Scopes show `a=1`,`b=2`,`c=3` (names vary with the
harness locals). The rejected wrong reading is: "I will only look after I am
inside asm." Looking once in the caller gives ground truth to compare after
step-into.

**Step 2 — F11 into `add3`.** Immediately open registers. `%rdi`/`%rsi`/`%rdx`
should still be 1/2/3. The rejected wrong reading is: "I already returned in
`%rax`, so args do not matter." Until you execute your adds, `%rax` may still
be stale junk from the caller; args are the story at entry.

**Step 3 — step until `ret`.** `%rax` becomes 6. Continue until the harness
prints `ok`. If `%rax` is wrong but args looked right at entry, the bug is in
your instruction sequence — not in the calling convention. That split saves
hours of "maybe SysV is different on my machine" panic.

## Distinctions worth keeping straight

- **Call-site break vs symbol break** — C context versus landing on the label.
- **Entry regs vs after your first `mov`** — photograph early or lose evidence.
- **Passing tests vs seeing the ABI** — green `ok` can hide a lucky clobber;
  live regs teach the contract.
- **F11 vs `stepi`** — coarse step into `.s` versus instruction trace.

## Check yourself

1. Which three registers hold `add3`'s arguments at entry?
2. Why is "Build current file" wrong for this exercise directory?
3. You step into `add3` and `%rax` is not yet the sum. Is that failure?
4. When would you switch from nvim-dap to gdb `stepi` for this binary?

## Key takeaways

- Live stops inside hand-written asm are how you verify the ABI you implemented.
- Break at the harness call, note C values, F11 in, read `%rdi`/`%rsi`/`%rdx`
  before you overwrite them; expect the sum in `%rax`.
- Multi-file `make` artifacts need Launch existing after a real link.
- Use gdb `stepi` when line-stepping blurs individual asm instructions.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F9, F11, `<leader>dc` / `de` / `dr`, `.s` + harness workflow
- `man 1 gdb` — `help info registers`, `help stepi`
- `man 1 as` — directives you already used in Track B

Now open `TASK.md` and do the practice.
