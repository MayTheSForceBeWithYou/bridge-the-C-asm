# 16 — Float / double in xmm* (practice)

Read `LESSON.md` first. This file is only the lab.

## Build and inspect

1. Implement `addf(float, float)`, `muld(double, double)`, and
   `mix(double a, int n)`.
2. `make asm disasm`.
3. Record XMM usage for args/returns, the GPR for the mixed int, and sample
   `*ss` / `*sd` opcodes.

## Done when

- You named the XMM registers used for FP args/returns from *your* listing.
- You showed where the `int` goes in `mix`.
- You spotted at least one scalar SSE arithmetic opcode in context.

## Lookup

`man 1 gcc`, `man 1 objdump` — discover FP ABI from output.
