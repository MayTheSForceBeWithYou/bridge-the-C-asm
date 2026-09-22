# 73 — Hand-written CFI so unwinding works

Debuggers and `backtrace(3)` do not guess your stack from vibes. They read
unwind tables — Call Frame Information (CFI) in `.eh_frame` — that describe how
to recover the Canonical Frame Address (CFA) and saved registers after each
instruction. This lesson forces you to author those directives because the stub
deliberately defeats frame-pointer heuristics. By the end you should update
`.cfi_def_cfa_offset` after every `%rsp` move, including the inbound +8 from
`call`, and verify unwind data with `readelf` / gdb `bt`.

## What this lesson asks of you

`main` → `level1` → `level2` already runs; `level2` calls `backtrace(3)` and
prints how many frames it found. Without CFI the count is about `1`. You add
`.cfi_*` directives so the unwinder climbs through your asm and CRT, targeting
`frames captured: 6`, and so gdb `bt` names `level2`, `level1`, and `main`.

## Why the stub cannot be guessed

None of the three functions sets up `%rbp` as a frame pointer. `level1` /
`level2` subtract a frame size held in a *register*, so a prologue scanner
cannot recover a constant offset. The only reliable story is `.eh_frame`.

**Navigation before you edit:**

```bash
readelf --debug-dump=frames-interp unwind_demo
# or: objdump --dwarf=frames unwind_demo
```

Expect emptiness / no useful FDE bodies for your functions. gdb will truncate
the backtrace and may invent a `0x0` frame — same missing metadata, different
consumer.

**Rejected wrong reading:** "`backtrace` is buggy." It is obeying missing CFI.

## CFA and `.cfi_def_cfa_offset`

The CFA is the stack address the unwinder treats as the base for this frame —
conceptually the `%rsp` value *before* the `call` that entered the function
(i.e. just after the return address was pushed, from the caller's perspective
the slot above the return address). Directives describe how to compute CFA from
the current `%rsp` (or `%rbp`).

### Field decoding — offsets are cumulative

| After this happens | Typical `.cfi_def_cfa_offset` | Why |
| ------------------ | ---------------------------- | --- |
| Function entry (return addr already pushed) | `8` | CFA = `%rsp + 8` |
| Extra `push` of 8 bytes | previous + 8 | `%rsp` moved |
| `subq $56, %rsp` | previous + 56 | **not** `56` alone |
| Example: entry + `subq $56` | `64` | 8 + 56 |

**Recognition rule:** the directive's offset is the distance from *current*
`%rsp` up to the CFA. Every instruction that moves `%rsp` needs a matching
`.cfi_def_cfa_offset` (or equivalent CFA rule) if unwinding from later
instructions must work.

**Rejected wrong reading:** "After `subq $56`, offset is 56." Add the
return-address skew (and any pushes) too.

Bracket each function with `.cfi_startproc` / `.cfi_endproc`.

Compiler `-O2` C without a frame pointer still unwinds because GCC emits these
directives for you — open any Track A `make asm` output and **search `.cfi_`**.

### What the directives look like in source (shape, not a solution)

```text
    .globl level2
    .type level2, @function
level2:
    .cfi_startproc
    .cfi_def_cfa_offset 8
    subq $56, %rsp
    .cfi_def_cfa_offset 64
    ...
    .cfi_endproc
```

Read that as: "on entry CFA is rsp+8; after the sub, CFA is rsp+64." The
immediate in `subq` is only one term in that sum.

## What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | prints frame count (≈1 before CFI; 6 after) |
| `readelf --debug-dump=frames-interp` | empty → populated FDEs |
| gdb `break backtrace`, `run`, `bt` | truncated → names `level2`/`level1`/`main` |

## How to verify after editing

1. Rebuild; run; look for `frames captured: 6`.
2. Re-dump frames; search for your function names in the interpreter output.
3. In gdb, break on `backtrace` (the libc function), `run`, `bt` — you want
   `level2`, `level1`, `main` visible without a bogus `0x0` truncation.
4. Symbol lines from `backtrace_symbols_fd` may appear *before* the count
   because of stdio buffering — that ordering quirk is not a CFI failure.

**Rejected wrong reading:** "My count printed after the symbols, so CFI
failed." Buffering reordered stdout; check the number and gdb `bt`.

## Frame-pointer experiment (task observation)

Try `push %rbp; mov %rsp, %rbp` *without* full CFI and see what gdb vs
`backtrace(3)` do on your machine. Often gdb improves via heuristics while
`backtrace(3)` still wants `.eh_frame`. That distinction is the lesson's point:
two consumers, one reliable metadata source.

## Worked example

**The situation.** `make run` prints a tiny frame count; gdb `bt` at `backtrace`
looks broken.

**Step 1 — measure the broken baseline.** Record the printed count and the
empty unwind dump.

**Step 2 — annotate each function.** Add start/endproc and update
`def_cfa_offset` after every `%rsp` change, remembering the inbound 8 bytes
from `call`. Walk each `sub`/`push`/`add` with the cumulative table above.

**Step 3 — re-measure.** Expect `frames captured: 6` (`level2`, `level1`,
`main`, plus three CRT frames). gdb should name your three functions.

**Step 4 — optional `strip` check.** `readelf -S` before/after `strip` — note
whether `.eh_frame` remains. Stripping symbols is not the same as deleting
unwind sections (exercise 52 territory).

## Distinctions worth keeping straight

- **Frame pointer heuristic vs `.eh_frame`** — `push %rbp; mov %rsp, %rbp` can
  fix *gdb* sometimes; `backtrace(3)` still wants CFI on many setups.
- **CFA offset vs local `sub` immediate** — the immediate is only one term.
- **`.eh_frame` vs `.debug_frame`** — exception/unwind aware vs debug; this
  exercise cares about what `backtrace` consumes.
- **`strip` vs unwind** — different sections; check `readelf -S`.
- **Stdout order vs frame count correctness** — buffering can reorder lines.

## Check yourself

1. What is the CFA, and why is offset after `subq $56` not simply `56`?
2. Why does `-O2` C unwind without `%rbp`?
3. Which section holds your directives' output? Does `strip` remove it?
4. What breaks in C++ / `-fexceptions` if a middle asm frame lacks CFI?
5. After entry + `push` (8) + `sub $56`, what `.cfi_def_cfa_offset` value
   matches the cumulative rule?
6. What command dumps interpreted frame info for you to search?

## Key takeaways

- Unwinders read CFI, not your intentions; hand-written asm must emit it when
  you omit frame pointers.
- Track CFA offsets through every `%rsp` change; include the inbound return
  address.
- `.cfi_startproc` / `.cfi_endproc` delimit functions; compilers show the pattern.
- Missing CFI truncates `backtrace` and gdb `bt` even when the program "runs."
- Verify with `readelf` frame dumps and gdb — not with hope.


## Where compilers show you the pattern

Pick any Track A exercise built with `make asm` at a level that still emits
CFI (common with GCC). Open the `.s` and search for `.cfi_startproc`. You will
see `.cfi_def_cfa_offset` and often `.cfi_offset` for saved registers next to
`push` instructions. Your hand-written annotations do not need every compiler
bell; they need correct CFA accounting through `%rsp` motion. Copy structure,
not opaque noise.


## Counting to six

The target `frames captured: 6` is typically your three asm frames plus three
CRT/libc frames above `main`. If you see 4–5, CFI may be partially wrong on
one level. If you see 1, unwinding still stops immediately. Do not hard-code a
print of 6 — fix the directives so `backtrace` *counts* six.

## Lookup (not the lesson)

- `info as` (node: CFI directives)
- `man 3 backtrace`, `man 1 readelf`, `man 1 objdump`, `man 1 gdb`
- gdb `help bt`; `NVIM_DAP.md` (Stacks panel)

Now open `TASK.md` and do the practice.
