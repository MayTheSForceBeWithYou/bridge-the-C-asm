# 73 — Hand-written CFI so unwinding works (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and run

1. `make && make run` — note the printed frame count (expect `1` before CFI).
2. Confirm no unwind data:
   `readelf --debug-dump=frames-interp unwind_demo` (or `objdump --dwarf=frames`).
3. gdb: `break backtrace`, `run`, `bt` — see truncation / `0x0`.
4. Add `.cfi_startproc` / `.cfi_endproc` to each function and
   `.cfi_def_cfa_offset` after every `%rsp`-moving instruction (inbound `call`
   already pushed 8).
5. `make && make run`; repeat steps 2–3.

## Done when

- `frames captured: 6` and exit 0; gdb `bt` names `level2`, `level1`, `main`.
- Count may print after symbol lines (stdio buffering) — that is fine.

## Observe

- CFA meaning; why offset after `subq $56` is not `56`.
- Why `-O2` C unwinds without a frame pointer (find `.cfi_*` in Track A asm).
- Which section holds CFI; effect of `strip` (`readelf -S`).
- Would `push %rbp; mov %rsp, %rbp` alone fix `backtrace(3)`? Try it.
- Cost of a missing asm CFI frame in C++ / `-fexceptions`.

## Lookup

`info as` (CFI directives), `man 3 backtrace`, `man 1 readelf`, `man 1 objdump`,
`man 1 gdb`, gdb `help bt`, `NVIM_DAP.md`.
