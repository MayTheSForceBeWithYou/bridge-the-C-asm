# 73 — Hand-written CFI so unwinding works (practice)

Read `LESSON.md` first — especially CFA offset accounting (entry +8) and how to
dump frames with `readelf`. This file is only the lab.

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

## Lookup

`info as` (CFI directives), `man 3 backtrace`, `man 1 readelf`, `man 1 objdump`,
`man 1 gdb`, gdb `help bt`, `NVIM_DAP.md`.
