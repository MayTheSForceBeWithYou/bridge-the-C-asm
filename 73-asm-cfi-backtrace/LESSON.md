# 73 — Hand-written CFI so unwinding works
<!-- concepts: c.p5.asm.cfi -->

Debuggers and `backtrace(3)` do not guess your stack from vibes. They read
unwind tables — Call Frame Information (CFI) in `.eh_frame` — that describe how
to recover the Canonical Frame Address (CFA) and saved registers after each
instruction. This lesson forces you to author those directives because the stub
deliberately defeats frame-pointer heuristics.

## What this lesson asks of you

`main` → `level1` → `level2` already runs; `level2` calls `backtrace(3)` and
prints how many frames it found. Without CFI the count is about `1`. You add
`.cfi_*` directives so the unwinder climbs through your asm and CRT, targeting
`frames captured: 6`, and so gdb `bt` names `level2`, `level1`, and `main`.

## Why the stub cannot be guessed

None of the three functions sets up `%rbp` as a frame pointer. `level1` /
`level2` subtract a frame size held in a *register*, so a prologue scanner
cannot recover a constant offset. The only reliable story is `.eh_frame`.
Confirm emptiness with `readelf --debug-dump=frames-interp` (or
`objdump --dwarf=frames`) before you edit.

gdb will truncate the backtrace and may invent a `0x0` frame — same missing
metadata, different consumer.

## CFA and `.cfi_def_cfa_offset`

The CFA is the stack address the unwinder treats as the base for this frame —
conceptually the `%rsp` value *before* the `call` that entered the function
(i.e. just after the return address was pushed, from the caller's perspective
the slot above the return address). Directives describe how to compute CFA from
the current `%rsp` (or `%rbp`).

Important accounting:

- On entry, the return address is already on the stack → CFA is `%rsp + 8`,
  often written `.cfi_def_cfa_offset 8` right after `.cfi_startproc` once you
  reason about entry.
- After `push` of 8 bytes, offset grows by 8.
- After `subq $56, %rsp`, offset grows by 56 more — so the directive's offset
  is **not** `56` alone; it is previous offset plus 56 (entry 8 + any pushes +
  56).

Every instruction that moves `%rsp` needs a matching `.cfi_def_cfa_offset` (or
equivalent CFA rule) if unwinding from later instructions must work.
`.cfi_startproc` / `.cfi_endproc` bracket each function.

Compiler `-O2` C without a frame pointer still unwinds because GCC emits these
directives for you — open any Track A `make asm` output and search `.cfi_`.

## Worked example

**The situation.** `make run` prints a tiny frame count; gdb `bt` at `backtrace`
looks broken.

**Step 1 — measure the broken baseline.** Record the printed count and the
empty unwind dump. The rejected wrong reading is: "`backtrace` is buggy." It is
obeying missing CFI.

**Step 2 — annotate each function.** Add start/endproc and update
`def_cfa_offset` after every `%rsp` change, remembering the inbound 8 bytes
from `call`. The rejected wrong reading is: "after `subq $56`, offset is 56."
Add the return-address skew (and pushes) too.

**Step 3 — re-measure.** Expect `frames captured: 6` (`level2`, `level1`,
`main`, plus three CRT frames). gdb should name your three functions without
truncation. Symbol lines from `backtrace_symbols_fd` may appear before the
count because of stdio buffering — that ordering quirk is not a CFI failure.

## Distinctions worth keeping straight

- **Frame pointer heuristic vs `.eh_frame`** — `push %rbp; mov %rsp, %rbp` can
  fix *gdb* sometimes; `backtrace(3)` still wants CFI on many setups. Try both
  and compare (task observation).
- **CFA offset vs local `sub` immediate** — the immediate is only one term.
- **`.eh_frame` vs `.debug_frame`** — exception/unwind aware vs debug; this
  exercise cares about what `backtrace` consumes.
- **`strip` vs unwind** — stripping symbols is not the same as stripping all
  unwind sections; check `readelf -S` before/after (exercise 52 territory).

## Check yourself

1. What is the CFA, and why is offset after `subq $56` not simply `56`?
2. Why does `-O2` C unwind without `%rbp`?
3. Which section holds your directives' output? Does `strip` remove it?
4. What breaks in C++ / `-fexceptions` if a middle asm frame lacks CFI?

## Key takeaways

- Unwinders read CFI, not your intentions; hand-written asm must emit it when
  you omit frame pointers.
- Track CFA offsets through every `%rsp` change; include the inbound return
  address.
- `.cfi_startproc` / `.cfi_endproc` delimit functions; compilers show the pattern.
- Missing CFI truncates `backtrace` and gdb `bt` even when the program "runs."

## Lookup (not the lesson)

- `info as` (node: CFI directives)
- `man 3 backtrace`, `man 1 readelf`, `man 1 objdump`, `man 1 gdb`
- gdb `help bt`; `NVIM_DAP.md` (Stacks panel)

Now open `TASK.md` and do the practice.
