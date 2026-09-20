# 75 — RIP-relative addressing in a PIE
<!-- concepts: c.p5.elf.pie, c.p5.asm.riprel -->

Position-Independent Executables (PIEs) load at a randomized base. Absolute
immediate addresses baked into instructions cannot be patched cheaply for every
load, so the toolchain forbids them for many cases. Hand-written asm must
address static data relative to `%rip` instead. This lesson makes that
constraint fail loudly, then work correctly.

## What this lesson asks of you

Implement the TODO in `riprel.s` so `%rdi` receives the address of `msg` via
RIP-relative addressing, then `call puts`. Compare link failure for an absolute
form, success under `-no-pie`, and disassembly of both. Exercise 49 watched ASLR
move a C binary; here you write code that survives the move.

## Why absolute immediates break PIE

A form like `movl $msg, %edi` asks for a 32-bit absolute relocation
(historically `R_X86_64_32` / similar). At link time for a PIE/DSO, the final
runtime address is not a fixed 32-bit link-time constant the loader will fix up
the way old non-PIE executables did. `make absolute` should fail with a
relocation error — read the relocation *name* in the message.

The RIP-relative form (`lea msg(%rip), %rdi` in AT&T) emits a PC-relative
relocation. At runtime, address = next instruction's `%rip` + displacement. No
load-time fixup of an absolute field is required for that reference: as the
whole image slides, `%rip` and `msg` slide together.

## What `%rip` means in the calculation

When the CPU applies a RIP-relative displacement, `%rip` is the address of the
*next* instruction (as usual on x86). `objdump -d` comments that resolve
`msg(%rip)` to an absolute address in the listing are showing the sum at the
linked addresses of that build — verify the displacement against the distance
from the following instruction to `msg`, not from the start of `lea`.

## Calls versus data references

`call puts` typically goes through the PLT (Procedure Linkage Table) in a
dynamically linked PIE: the call target is a stub that resolves libc, not a
direct absolute immediate to `puts` in your text. That is why "call a function"
and "load address of my `.rodata` string" need different treatments — exercise
48 is the PLT deep dive; here you only need to notice the difference in
disassembly.

## Worked example

**The situation.** The stub passes NULL to `puts` and crashes until you compute
`msg`'s address correctly.

**Step 1 — RIP-relative load.** Use `lea msg(%rip), %rdi` (or equivalent), then
call `puts`. `make && make run` should print the message; `readelf -h` should
show type `DYN` for the PIE. The rejected wrong reading is: "`mov $msg, %rdi`
is fine if I link somehow." On default PIE toolchains it is not.

**Step 2 — force the absolute mistake.** `make absolute` and read the linker
error's relocation name. Then `make absolute-nopie && ./riprel.abs` to see the
same absolute object work as `EXEC`. The rejected wrong reading is: "PIE vs
non-PIE is only an ASLR policy switch." It also changes which relocations are
legal in your asm.

**Step 3 — compare listings.** `make disasm` vs `objdump -d riprel.abs` on
`main`: one shows RIP-relative, the other an absolute immediate. Run the PIE
twice under different ASLR conditions if available (`setarch -R` vs plain) and
confirm behavior persists while load base can move.

## Distinctions worth keeping straight

- **PIE `DYN` vs non-PIE `EXEC`** — `readelf -h` Type field.
- **RIP-relative data address vs PLT call** — both are "position independent,"
  different mechanisms.
- **Link-time relocation failure vs runtime NULL crash** — absolute form often
  never links as PIE; forgetting `lea` links but crashes in `puts`.
- **Disassembly comment address vs runtime ASLR address** — comments are for
  that linked image layout.

## Check yourself

1. Which relocation shows up for `movl $msg, %edi` versus the RIP-relative
   form? (`objdump -r` on the objects.)
2. Why can a 32-bit absolute relocation not be fixed at load for a PIE the way
   RIP-relative needs no absolute fixup?
3. Is `%rip` the start or the end of the current instruction when the
   displacement applies?
4. Why does `call puts` not need the same `lea` treatment as `msg`?

## Key takeaways

- PIE asm addresses static data with `thing(%rip)`, not `$thing` immediates.
- Absolute 32-bit relocations fail at link for PIEs; `-no-pie` is a different
  (legacy) contract.
- RIP-relative uses the next instruction's address plus a displacement.
- Libc calls usually go via PLT; that is not the same as materializing a data
  address.

## Lookup (not the lesson)

- `info as` (node: i386-Memory)
- `man 1 ld`, `man 1 objdump`, `man 1 readelf`
- `man 1 gcc` (`-no-pie`, `-fPIE`), `man 8 setarch`, `man 3 puts`

Now open `TASK.md` and do the practice.
