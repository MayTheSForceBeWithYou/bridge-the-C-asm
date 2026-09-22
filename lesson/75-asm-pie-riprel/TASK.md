# 75 — RIP-relative vs absolute in a PIE (practice)

Read `LESSON.md` first — especially how to decode a `lea …(%rip)` `objdump`
line and the absolute-vs-RIP relocation table. This file is only the lab.

## Build and run

1. Implement the TODO in `riprel.s` so `%rdi` gets `msg` via RIP-relative
   addressing; `make && make run` (stub passes NULL until you do).
2. `make absolute` — read the link error / relocation name.
3. `make absolute-nopie && ./riprel.abs`.
4. Compare:
   ```
   make disasm && grep -A2 '<main>' riprel.lst
   objdump -d riprel.abs | grep -A6 '<main>:'
   ```
5. Confirm PIE is `DYN` (`readelf -h`). Optional ASLR contrast:
   `setarch -R ./riprel` vs plain.

## Done when

- `./riprel` prints the message, exits 0, is PIE (`DYN`).
- `make absolute` fails at link; `./riprel.abs` works as `EXEC`.
- You can decode one RIP-relative line into address / displacement / `#` target.

## Lookup

`info as` (i386-Memory), `man 1 ld`, `man 1 objdump`, `man 1 readelf`,
`man 1 gcc`, `man 8 setarch`, `man 3 puts`.
