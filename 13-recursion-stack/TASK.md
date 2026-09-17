# 13 — Recursion and stack growth (Track A)

## Goal
Watch a recursive factorial/fib deepen the stack under `gdb`.

## Do
1. Implement `long fac(long n);` recursively (no tail form required).
2. `make` at `-O0`, then:
   ```
   gdb -q ./prog
   (gdb) break fac
   (gdb) run
   (gdb) stepi
   (gdb) info registers rsp rbp
   ```
   Step through a few recursive entries and watch `rsp`.

## Observe / answer
- How much does `rsp` move per call?
- What is saved on the stack each time (return address, `rbp`, locals)?
- Confirm with `disassemble fac` inside gdb (`help disassemble`).

## Refs
`man 1 gdb`, gdb `help disassemble`, `help stepi`, `help info registers`,
`man 1 objdump`
