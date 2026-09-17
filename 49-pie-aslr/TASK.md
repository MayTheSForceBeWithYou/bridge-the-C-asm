# 49 — PIE vs -no-pie / ASLR (Track E)

## Goal
`readelf -h`: PIE is `Type: DYN`, `-no-pie` is `Type: EXEC`. Observe ASLR
base change across gdb runs.

## Do
1. Build both:
```
make
gcc -ggdb -O0 -no-pie -o prog.nopie prog.c
readelf -h ./prog | grep Type
readelf -h ./prog.nopie | grep Type
```
2. gdb ASLR check on PIE binary:
```
gdb -q ./prog
(gdb) break main
(gdb) run
(gdb) info proc mappings
(gdb) print/x $rip
(gdb) kill
(gdb) run
(gdb) print/x $rip
(gdb) quit
```
3. Did the text base / `$rip` change between runs? Repeat with
   `set disable-randomization on` and compare.

## Success
You recorded DYN vs EXEC and saw ASLR move the PIE base (unless disabled).

## Refs
`man 1 readelf`, `man 5 elf`, `man 1 gdb`, `man 1 gcc`
