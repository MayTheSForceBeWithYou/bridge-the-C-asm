# 36 — Watching a loop accumulate

A loop is where register state changes on every iteration while the *source*
looks like the same three lines. Debuggers earn their keep when you can watch
an accumulator climb from 0 toward `sum_to(10) == 55` without inserting
`printf`. gdb's TUI (`layout regs` / `layout asm`) and nvim-dap's virtual text
plus Scopes are two cameras on that motion — neither is a clone of the other.

## What this lesson asks of you

Implement `sum_to`, break at entry, and step the loop body until you have *seen*
the running total update live. Prefer nvim-dap for in-editor virtual text; use
gdb TUI if you want the classic split asm/regs panes. The competence is reading
iteration state, not memorizing TUI keybindings.

`TASK.md` is practice. Man/`info` pages document `layout` spelling; they do not
teach you what "the accumulator moved" looks like.

## Two UIs, one loop

**nvim-dap.** F9 on `sum_to`, Launch existing → `./prog`, then F10/F11. Virtual
text (nvim-dap-virtual-text) paints values at the end of lines; Scopes refreshes
locals; optional Watches pin the accumulator or `n`. This feels like the editor
is narrating each trip through the body. Toggle panes with `<leader>du` if they
did not appear on launch.

**gdb TUI.** `layout regs` and `layout asm` (or `layout split`) put registers
and disassembly on screen while `stepi` advances one instruction. If the TUI
glitches, `tui disable` / `tui enable`. You are watching the same process; the
asm pane makes the jump-back of the loop obvious when the instruction pointer
returns to the add.

Neither view is "more correct." dap-ui is not a 1:1 `layout regs` clone —
register groups live under Scopes, not in a dedicated curses pane. Choose based
on whether you want C-line narration or instruction flicker.

## Step grain matters

F10 steps over a source line; F11 steps into calls. Inside a tight loop with no
calls, both may land on the same lines repeatedly while the accumulator
changes — that repetition is success, not a stuck debugger. gdb `stepi` shows
the add/jcc dance underneath. For this lesson, seeing the increment at all is
enough; you do not need a perfect `stepi` transcript unless the asm view is how
you prefer to learn.

Compare mentally to lesson 35: there, per-instruction grain was mandatory to
count `%rsp` drops. Here, line grain is usually enough because the question is
"did the total climb?" not "which opcode added?"

## Worked example

**The situation.** `sum_to(10)` should return 55. You break at `sum_to`, run,
and prepare to step.

**Step 1 — entry.** Note `n` is 10. Accumulator starts at 0 (or whatever your
implementation uses). The rejected wrong reading is: "I will continue until
exit and only look at the return value." That skips the whole point of a loop
lab.

**Step 2 — one full iteration visible.** F10 (or a few `stepi`) until the
accumulator becomes 1, then 2. Virtual text / regs should move. The rejected
wrong reading is: "the debugger is slow so I continue." Speed is not the skill;
observation is.

**Step 3 — optional TUI.** `layout asm`, `stepi` until you recognize the
backward branch. Then `continue` to finish and confirm `ok`. If TUI corrupts
the terminal, disable it and keep stepping in the normal CLI — the registers
still update with `info registers` after each `stepi`.

## Distinctions worth keeping straight

- **Virtual text vs TUI regs** — inline C values versus live GPR pane.
- **F10 loop stepping vs `stepi`** — line cadence versus insn cadence.
- **Watch expression vs reading Scopes each stop** — pinned versus ambient.
- **Seeing 55 in `%rax` at return vs watching the climb** — product versus
  process.

## Check yourself

1. What is `sum_to(10)` supposed to return, and why does stepping matter if the
   test already checks it?
2. Name one thing dap-ui shows well that is *not* identical to `layout regs`.
3. When would you reach for `stepi` inside this loop instead of F10?
4. What key toggles dap-ui if the panes did not open on launch?

## Key takeaways

- Loops teach live state: watch the accumulator change, do not only assert the
  final sum.
- nvim-dap virtual text/Scopes and gdb TUI are alternate cameras — use either.
- Match step grain to the question (line vs instruction).
- `<leader>du` recovers dap-ui; TUI has its own enable/disable escape hatches.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F10/F11, Watches, `<leader>du`
- `man 1 gdb` / `info gdb` — TUI node; `help layout`, `help stepi`

Now open `TASK.md` and do the practice.
