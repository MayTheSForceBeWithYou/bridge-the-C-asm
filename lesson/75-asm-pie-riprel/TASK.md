# 75 — RIP-relative vs absolute in a PIE (practice)

Read `LESSON.md` first. This file is only the lab.

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
5. Run the PIE twice; confirm it works as `DYN` (`readelf -h`). Optional ASLR
   contrast: `setarch -R ./riprel` vs plain.

## Done when

- `./riprel` prints the message, exits 0, is PIE (`DYN`).
- `make absolute` fails at link; `./riprel.abs` works as `EXEC`.

## Observe

- Relocations for absolute vs RIP-relative (`objdump -r`).
- Why absolute 32-bit cannot be load-fixed for PIE.
- Whether `%rip` is start or next instruction (check disassembly comments).
- Why `puts` via PLT differs from a data address (ex. 48).

## Lookup

`info as` (i386-Memory), `man 1 ld`, `man 1 objdump`, `man 1 readelf`,
`man 1 gcc`, `man 8 setarch`, `man 3 puts`.
