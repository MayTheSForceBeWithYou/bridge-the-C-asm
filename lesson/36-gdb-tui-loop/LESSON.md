# 36 — Watching a loop accumulate

A loop is where register state changes on every iteration while the *source*
looks like the same three lines. Debuggers earn their keep when you can watch
an accumulator climb from 0 toward `sum_to(10) == 55` without inserting
`printf`. gdb's TUI (`layout regs` / `layout asm`) and nvim-dap's virtual text
plus Scopes are two cameras on that motion — neither is a clone of the other.
By the end you should tell a **register / locals pane** update from an
**address** you might dump, and match step grain to the question.

## What this lesson asks of you

Implement `sum_to`, break at entry, and step the loop body until you have *seen*
the running total update live. Prefer nvim-dap for in-editor virtual text; use
gdb TUI if you want the classic split asm/regs panes. The competence is reading
iteration state, not memorizing TUI keybindings.

`TASK.md` is practice. Man/`info` pages document `layout` spelling; they do not
teach you what "the accumulator moved" looks like.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | build `./prog` with `-ggdb -O0` | ready to debug |
| `make run` | execute harness | `ok` when `sum_to(10)==55` |

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

### Register panes versus addresses (recognition)

| View | What moves each iteration | What it is *not* |
| ---- | ------------------------- | ---------------- |
| dap-ui Scopes / virtual text | Named locals (`sum`, `i`, `n`) and optionally GPRs | A hex dump of memory |
| gdb `layout regs` | Live GPR / flags pane | Identical layout to dap-ui |
| `%rip` / asm highlight | Which instruction is current | The accumulator's *value* unless that value sits in a watched reg |
| `<leader>dm` / `x/` | Bytes at an address you chose | Automatic "follow the loop" |

**Recognition rule:** when the accumulator lives in a register or a named local,
you watch the **pane** / virtual text. Open memory view only if you need bytes
at a known address (spilled slot, array). Mixing "I watched `$rsp`" with "I
watched the sum climb" is how people leave this lab without seeing the
increment.

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

**F11 ≠ `stepi`** still holds. Do not write lab notes that claim instruction
steps from F11.

### Keys for the dap path (exact)

| Key | Action |
| --- | ------ |
| F9 | Break on `sum_to` |
| F5 / `<leader>dc` | Launch existing → `./prog` |
| F10 / F11 | Step the loop body |
| `<leader>du` | Toggle dap-ui |
| Watches pane | Pin accumulator / `n` (display, not data watch) |
| `<leader>dt` | Terminate |

## Worked example

**The situation.** `sum_to(10)` should return 55. You break at `sum_to`, Launch
existing → `./prog`, and prepare to step.

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

**Step 4 — optional Watch.** Pin the accumulator expression in dap-ui Watches
so you are not hunting virtual text each stop — still a display, not a data
watchpoint (lesson 34).


## What "the accumulator moved" looks like in each UI

**Virtual text.** After F10, a value annotation appears (or updates) at the end
of a source line — often the new `sum` or `i`. If annotations do not appear,
check that nvim-dap-virtual-text is active and that you actually stopped on a
new line (not continued to exit).

**Scopes.** Expand Locals. The row for your accumulator should change between
stops. If it shows `<optimized out>` you may have launched an unexpected `-O2`
build — this lesson's Makefile defaults should keep `-O0`.

**TUI regs.** After `layout regs` and `stepi`, watch a GPR you identified as the
accumulator (often whatever holds the running total in the asm). The pane
flickers; `%rip` moves; a backward jump in `layout asm` is the loop edge.

**Rejected wrong reading:** "I ran `layout regs` once and continued to the end."
TUI without stepping is a screensaver. The skill is seeing values *change*.

### Optional: pin a Watch without thinking it is `watch`

In dap-ui Watches, add `sum` (or whatever you named the total). Each F10
refreshes it. That is the lesson-34 "watched expression" — perfect here, still
not a hardware watchpoint. If you later confuse the two, re-read lesson 34's
three-way table.



## When the loop looks "stuck" on the same source lines

F10 landing on the same three lines repeatedly is normal: that *is* the loop.
Watch the *values* (virtual text / Scopes / regs), not only the line number
highlight. If values freeze while you believe you are stepping, check whether
you are actually stopped in a tight path that does not update your watched
name (wrong variable) or whether the session continued to exit.



## Makefile / launch reminder

Same as other Track C labs: `make` first, then Launch existing → `./prog` under
nvim-dap. If you use gdb TUI only, `gdb -q ./prog` then `break sum_to`, `run`,
`layout regs`, `layout asm`, and step. Either path satisfies "saw the
increment live"; neither path is excused from implementing `sum_to` correctly
first.



**Rejected wrong reading (pane vs address):** "I dumped `$rsp` with `<leader>dm`,
so I watched the loop accumulate." Unless your accumulator spilled to a stack
slot you intentionally examined, the sum lives in a local / register pane.
Dumping the stack pointer without a reason is a different lab's skill (35).


## Distinctions worth keeping straight

- **Virtual text vs TUI regs** — inline C values versus live GPR pane.
- **Register / local panes vs memory addresses** — watch the sum where it lives;
  dump memory only when you need bytes.
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
5. Your accumulator is a local `sum`. Should your first move be `<leader>dm` or
   Scopes / virtual text? Why?
6. Does F11 guarantee one machine instruction per keypress in this loop?

If fuzzy, re-read the pane-vs-address table before opening `info gdb`.

## Key takeaways

- Loops teach live state: watch the accumulator change, do not only assert the
  final sum.
- nvim-dap virtual text/Scopes and gdb TUI are alternate cameras — use either.
- Register panes and locals are where loop totals usually appear; memory view is
  a different tool.
- Match step grain to the question (line vs instruction); F11 is still not
  `stepi`.
- `<leader>du` recovers dap-ui; TUI has its own enable/disable escape hatches.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F10/F11, Watches, `<leader>du`
- `man 1 gdb` / `info gdb` — TUI node; `help layout`, `help stepi`

Now open `TASK.md` and do the practice.
