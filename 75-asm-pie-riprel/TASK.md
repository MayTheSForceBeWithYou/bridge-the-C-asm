# 75 — RIP-relative vs absolute addressing in a PIE (Track B / E)

## Goal
Feel the constraint PIE puts on hand-written asm: you may not name an absolute
address, so you address data relative to `%rip`. Exercise 49 watched ASLR move
a C binary; this one makes you write code that survives the move.

## Do
1. Implement the TODO in `riprel.s` so `%rdi` holds the address of `msg`
   computed from the instruction pointer, then `make && make run`.
   The stub passes NULL to `puts` and crashes until you do.
2. See the absolute form fail to link:
   ```
   make absolute
   ```
   Read the error. Note the relocation name it complains about.
3. Same object, different link mode:
   ```
   make absolute-nopie && ./riprel.abs
   ```
4. Compare addressing in the disassembly of both binaries:
   ```
   make disasm && grep -A2 '<main>' riprel.lst
   objdump -d riprel.abs | grep -A6 '<main>:'
   ```
5. Run your PIE twice and confirm the load address moves while the code keeps
   working: `./riprel; ./riprel` under
   `setarch -R ./riprel` vs plain, or check `readelf -h` for the type.

## Success
`./riprel` prints the message and exits 0 as a PIE (`readelf -h` shows
`DYN`), `make absolute` fails at link, and `./riprel.abs` works as `EXEC`.

## Observe / answer
- Which relocation does the assembler emit for `movl $msg, %edi`, and which for
  the RIP-relative form? (`objdump -r riprel.abs.o riprel.o`)
- Why can a 32-bit absolute relocation never be fixed up at load time for a PIE,
  while the RIP-relative one needs no fixup at all?
- What is `%rip` when the displacement is applied — the start of the current
  instruction or the next one? Verify against the disassembly's resolved
  comment.
- Does calling `puts` need the same treatment? Look for a PLT entry
  (exercise 48) and explain the difference between code and data references.

## Refs
`info as` (node: i386-Memory), `man 1 ld`, `man 1 objdump`, `man 1 readelf`,
`man 1 gcc` (`-no-pie`, `-fPIE`), `man 8 setarch`, `man 3 puts`.
