# 33 — Live ABI in hand-written asm

You already wrote (or will write) `add3` in assembly: three `long` arguments in
`%rdi`/`%rsi`/`%rdx`, sum in `%rax`. Compiling the harness proves the contract
when the test passes. Debugging proves it *while* the instructions run — before
you overwrite an argument register, and after the sum lands in `%rax`. That gap
between "tests green" and "I saw the ABI" is what this lesson closes. By the end
you should photograph entry regs from a harness call site, tell register-pane
integers from pointer addresses, and know when F11 is too coarse for asm.

## What this lesson asks of you

Launch the Makefile-linked `./add3_test`, stop at the C call site, step into
your `.s`, and read registers the way a reviewer would. The competence is
joining Track B's static ABI story to a live stop inside code *you* authored,
not only inside GCC's output.

`TASK.md` is practice. `NVIM_DAP.md` spells keys; this lesson teaches what to
look for when you press them.

## What `make` is naming for you

| You type | Rough equivalent | File / check afterward |
| -------- | ---------------- | ---------------------- |
| `make` | assemble `add3.s`, compile harness, link with `-ggdb` | `./add3_test` |
| `make run` | run the test harness | prints `ok` |

`add3_test` is harness `.c` plus `add3.s`. The Makefile's `NAME` is the binary
you launch — not `harness` alone, and not the stem of whichever buffer is open.

## Why break on the call site first

A breakpoint on the asm symbol `add3` works when the debugger resolves the
symbol. A breakpoint on the line in `harness.c` that **calls** `add3` is often
clearer: you still have C names `a`,`b`,`c` in Scopes, you see the call about to
fire, then **F11** transfers into the callee.

### Navigation before interpretation

1. Open `harness.c` (not only the `.s`).
2. Search for the `add3(` call inside `expect` (or the helper that invokes it).
3. **F9** on that call line — red marker on the *call site*, not somewhere random
   in `main`.
4. `<leader>dc` → **"Launch existing executable"** → `./add3_test`.
5. When stopped, read Scopes / `<leader>de` *before* F11, then F11 into asm.

After the `call`, the CPU has pushed the return address and jumped to `add3`.
Argument registers should still hold the three longs until your first
instructions clobber them. Photograph that window early. If your first
instruction is `movq %rdi, %rax` (or similar), one step later `%rdi` may still
be intact — or not, depending on what you wrote. Entry is the reliable moment.

**Rejected wrong reading:** "I will only look after I am inside asm." Looking
once in the caller gives ground truth to compare after step-into.

## How to read register groups in dap-ui

CodeLLDB exposes register groups under Scopes (toggle dap-ui with `<leader>du`
if needed). You want the general-purpose set. Treat each row as a named ABI
slot:

| Register | At `add3` entry for three `long`s | After a correct body |
| -------- | --------------------------------- | -------------------- |
| `%rdi` | first arg `a` | may be clobbered |
| `%rsi` | second arg `b` | may be clobbered |
| `%rdx` | third arg `c` | may be clobbered |
| `%rax` | often stale junk from caller | the sum |

Confirm `%rdi`,`%rsi`,`%rdx` match the values you saw for `a`,`b`,`c` at the
call site (or via `<leader>de`). Step with F10/F11 and watch `%rax` become the
sum. Prefer Scopes and hover before diving into the DAP REPL (`<leader>dr`);
REPL is optional power, not the first tool.

### Register pane vs address (recognition)

| What you see | Meaning for this lab |
| ------------ | -------------------- |
| `%rdi = 1` (small integer) | Argument *value* — not a pointer to chase |
| `%rdi = 0x7fffffffe1a0` (pointer-shaped) | Wrong expectation for `add3(long,long,long)` at entry — or you are in a different frame |
| Named `a = 1` in Locals | Debug-info view of the same story as `%rdi` at the call site |
| Memory view at `$rsp` | Stack slots (return address, etc.) — optional; not where `a`/`b`/`c` live for a pure register ABI |

**Recognition rule:** for integer/long args passed in registers, the register
pane *is* the payload. Do not paste `1` into `<leader>dm` looking for the
number one as a byte pattern unless you intentionally spilled. Lesson 32 needed
memory view because the *array* lived behind a pointer; `add3` does not.

**Rejected wrong reading:** "I already returned in `%rax`, so args do not
matter." Until you execute your adds, `%rax` may still be stale junk from the
caller; args are the story at entry.

## F11 vs `stepi` inside `.s`

Honest limit: F10/F11 are usually **source/line** steps under CodeLLDB. A
compact asm function may be a handful of instructions behind one or two "lines"
of `.s`; F11 might appear to "jump" through them depending on debug info.

When you need every `addq` visible:

```text
(gdb) break add3
(gdb) run
(gdb) info registers rdi rsi rdx rax
(gdb) stepi
(gdb) info registers rax
```

Same skill, tighter grain. Lesson 35 depends on that honesty for `%rsp`.
Claiming F11 walked each insn is a recognition failure even if `%rax` ended
right.

Keys for the dap half of the workflow (exact):

| Key | Use here |
| --- | -------- |
| F9 | Call-site breakpoint |
| F5 / `<leader>dc` | Launch / continue |
| F11 | Step into `add3` (source grain) |
| F10 | Step over once inside if lines map |
| `<leader>de` | Hover `a`/`b`/`c` |
| `<leader>dt` | Terminate when done |

## Launch existing, not "Build current file"

nvim-dap's "Build current file and launch" would `gcc -g` only the open buffer
and miss your assembly (or miss the harness). Always `make` first, then
**"Launch existing executable"** → `./add3_test`. Single-file throwaways can use
Build current file; this directory is not one of those.

**Rejected wrong reading:** "F5 rebuilt my program so I am fine." F5 starts the
*selected launch config*. If that config is Build current file, you silently
dropped half the program.

## Worked example

**The situation.** `expect` calls `add3(1, 2, 3)` wanting `6`. You F9 on that
call, Launch existing → `./add3_test`, stop in the harness.

**Step 1 — caller view.** Scopes show `a=1`,`b=2`,`c=3` (names vary with the
harness locals). Hover with `<leader>de` if the pane is crowded. Record them —
these are your ground truth.

**Step 2 — F11 into `add3`.** Immediately open the register group. `%rdi` /
`%rsi` / `%rdx` should still be 1/2/3. If they are wrong *at entry*, the bug is
in the caller or you broke on the wrong frame — not yet in your add sequence.

**Step 3 — step until `ret`.** `%rax` becomes 6. Continue until the harness
prints `ok`. If `%rax` is wrong but args looked right at entry, the bug is in
your instruction sequence — not in the calling convention. That split saves
hours of "maybe SysV is different on my machine" panic.

**Step 4 — optional `stepi` pass.** If F11 blurred the body, switch to gdb and
walk each insn once so you see when `%rax` updates relative to each `addq`.


## Separating ABI bugs from opcode bugs (decision table)

| What you see at entry | What you see after stepping | Where to look |
| --------------------- | --------------------------- | ------------- |
| `%rdi/%rsi/%rdx` wrong | (irrelevant yet) | Call site, wrong binary, wrong frame |
| Args correct | `%rax` wrong | Your `.s` sequence |
| Args correct | `%rax` correct, harness still fails | Different expect triple; break again on the failing call |
| Cannot F11 into `.s` | — | Launch existing? symbols? break on call line? |

This table is the practical payoff of live debugging: green tests can hide
lucky register reuse; live entry photos cannot.



## What `%rax` looking "right too early" means

Sometimes `%rax` already holds `6` at entry because the caller happened to leave
a useful value there — luck, not your adds. Always photograph args at entry;
treat `%rax` as meaningful *after* your sequence. Lucky `%rax` plus wrong args
is still an ABI mystery you have not solved.


## Distinctions worth keeping straight

- **Call-site break vs symbol break** — C context versus landing on the label.
- **Entry regs vs after your first `mov`** — photograph early or lose evidence.
- **Passing tests vs seeing the ABI** — green `ok` can hide a lucky clobber;
  live regs teach the contract.
- **F11 vs `stepi`** — coarse step into `.s` versus instruction trace.
- **Register-pane integers vs memory addresses** — args here are values in GPRs.
- **Launch existing vs Build current file** — real `make` link versus incomplete
  binary.

## Check yourself

Close this file and answer from memory, then verify at a live stop:

1. Which three registers hold `add3`'s arguments at entry? Which holds the sum
   after a correct return path?
2. Why is "Build current file" wrong for this exercise directory?
3. You step into `add3` and `%rax` is not yet the sum. Is that failure?
4. When would you switch from nvim-dap to gdb `stepi` for this binary?
5. You want C names `a`,`b`,`c` visible at the stop. Do you F9 on `add3:` in the
   `.s` or on the call in `harness.c`?
6. At entry, `%rdi` shows `1`. Is that an address you should paste into
   `<leader>dm`?

If fuzzy, re-read — then look up key spellings in `NVIM_DAP.md`.

## Key takeaways

- Live stops inside hand-written asm are how you verify the ABI you implemented.
- Break at the harness call, note C values, F11 in, read `%rdi`/`%rsi`/`%rdx`
  before you overwrite them; expect the sum in `%rax`.
- Multi-file `make` artifacts need Launch existing after a real link.
- Use gdb `stepi` when line-stepping blurs individual asm instructions.
- Register panes hold the ABI story for integer args; do not confuse them with
  pointer addresses from pointer labs.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F9, F11, `<leader>dc` / `de` / `dr`, `.s` + harness workflow
- `man 1 gdb` — `help info registers`, `help stepi`
- `man 1 as` — directives you already used in Track B

Now open `TASK.md` and do the practice.
