# 73 — Hand-written CFI so unwinding works (Track B / C)

## Goal
Learn what a stack walker actually needs from your assembly. `main` → `level1`
→ `level2` already runs; `level2` asks libc to walk the stack with
`backtrace(3)` and prints how many frames it found. The count is your score.

## Setup
None of the three functions sets up `%rbp`, and `level1` / `level2` subtract a
frame size held in a register, so no prologue-reading heuristic can recover it.
The only reliable source is `.eh_frame` — which the stub does not have.

## Do
1. `make && make run`. Read the printed count. It should be `1`: the unwinder
   cannot get out of `level2`.
2. Confirm there is no unwind data yet:
   `readelf --debug-dump=frames-interp unwind_demo` (or `objdump --dwarf=frames`).
3. See gdb fail the same way — it truncates and then invents a `0x0` frame:
   ```
   gdb -q ./unwind_demo
   (gdb) break backtrace
   (gdb) run
   (gdb) bt
   ```
4. Add `.cfi_startproc` / `.cfi_endproc` to each function and a
   `.cfi_def_cfa_offset` after every instruction that moves `%rsp`. Remember
   the `call` that entered the function already pushed 8 bytes.
5. `make && make run`, then repeat steps 2 and 3.

## Success
`frames captured: 6` — `level2`, `level1`, `main`, and three CRT frames
(`__libc_start_call_main`, `__libc_start_main`, `_start`). gdb `bt` names
`level2`, `level1`, and `main` with no truncation. Exit status 0.

Ordering note: the symbol lines come from `backtrace_symbols_fd`, which writes
to fd 1 directly, while the count goes through buffered `printf` — so the count
prints last. That is the stdio buffering from Track A, not a bug.

## Observe / answer
- What is the CFA, and why is `.cfi_def_cfa_offset` after `subq $56` not `56`?
- Why does compiler-generated `-O2` C code unwind fine with no frame pointer?
  Find its `.cfi_*` lines in any Track A `make asm` output.
- Which section holds what your directives produced? Does `strip` remove it?
  (Compare with exercise 52, and check `readelf -S` before and after.)
- `push %rbp; mov %rsp, %rbp` would also fix the gdb backtrace. Would it fix
  `backtrace(3)`? Try it and explain the difference.
- What breaks in a C++ or `-fexceptions` program if an asm frame in the middle
  has no CFI?

## Refs
`info as` (node: CFI directives), `man 3 backtrace`, `man 1 readelf`,
`man 1 objdump`, `man 1 gdb`, gdb `help bt`, `NVIM_DAP.md` (Stacks panel).
