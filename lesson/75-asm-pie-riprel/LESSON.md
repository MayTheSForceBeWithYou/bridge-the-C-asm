# 75 — RIP-relative addressing in a PIE

Position-Independent Executables (PIEs) load at a randomized base. Absolute
immediate addresses baked into instructions cannot be patched cheaply for every
load, so the toolchain forbids them for many cases. Hand-written asm must
address static data relative to `%rip` instead. This lesson makes that
constraint fail loudly, then work correctly. By the end you should decode
`msg(%rip)` versus `$msg`, read a relocation name from a link error, and
separate RIP-relative *data* addressing from PLT *calls*.

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
relocation error — **read the relocation name** in the message; that name is the
recognition artifact.

The RIP-relative form (`lea msg(%rip), %rdi` in AT&T) emits a PC-relative
relocation. At runtime, address = next instruction's `%rip` + displacement. No
load-time fixup of an absolute field is required for that reference: as the
whole image slides, `%rip` and `msg` slide together.

| Form | Typical relocation idea | PIE default link |
| ---- | ----------------------- | ---------------- |
| `mov $msg, %rdi` / `movl $msg, %edi` | absolute (e.g. `R_X86_64_32`) | often **fails** |
| `lea msg(%rip), %rdi` | PC-relative (e.g. `R_X86_64_PC32` / GOT variants) | **ok** |

**Rejected wrong reading:** "`mov $msg, %rdi` is fine if I link somehow." On
default PIE toolchains it is not.

**Rejected wrong reading:** "PIE vs non-PIE is only an ASLR policy switch." It
also changes which relocations are legal in your asm.

## What `%rip` means in the calculation

When the CPU applies a RIP-relative displacement, `%rip` is the address of the
*next* instruction (as usual on x86). `objdump -d` comments that resolve
`msg(%rip)` to an absolute address in the listing are showing the sum at the
linked addresses of that build — verify the displacement against the distance
from the **following** instruction to `msg`, not from the start of `lea`.

### How to read one RIP-relative line

Shape from this track (addresses vary with layout; columns do not):

```text
    11a8:	48 8d 3d 61 0e 00 00 	lea    0xe61(%rip),%rdi        # 2010 <msg>
```

| Column / token | Example | What it is |
| -------------- | ------- | ---------- |
| Instruction address | `11a8:` | Where the `lea` lives |
| Raw bytes | `48 8d 3d 61 0e 00 00` | Encoding including displacement |
| Displacement in mnemonic | `0xe61(%rip)` | offset from *next* RIP |
| `#` annotation | `# 2010 <msg>` | `objdump` already added: resolved target in this image |

**Recognition rule (same trap as lesson 01):** neither `0xe61` nor `2010` is
"the instruction address." The instruction address is `11a8`. `0xe61` is the
encoded offset; `2010` is the resolved symbol address in this linked layout.

**Rejected wrong reading:** "`0xe61` is where `msg` lives." It is the
displacement; `msg` lives at the `#` target (in this build), or at
`next_rip + 0xe61` at run time under ASLR.

## Calls versus data references

`call puts` typically goes through the PLT (Procedure Linkage Table) in a
dynamically linked PIE: the call target is a stub that resolves libc, not a
direct absolute immediate to `puts` in your text. That is why "call a function"
and "load address of my `.rodata` string" need different treatments — exercise
48 is the PLT deep dive; here you only need to notice the difference in
disassembly.

| Goal | Typical asm | Mechanism |
| ---- | ----------- | --------- |
| Address of `msg` | `lea msg(%rip), %rdi` | RIP-relative data ref |
| Call `puts` | `call puts@plt` / `call puts` | PLT / linker stub |

## What `make` maps to

| You type | Result | Inspect |
| -------- | ------ | ------- |
| `make && make run` | PIE binary `./riprel` | prints message; `readelf -h` → `DYN` |
| `make absolute` | link **fails** | read relocation name |
| `make absolute-nopie && ./riprel.abs` | non-PIE works | `readelf -h` → `EXEC` |
| `make disasm` vs `objdump -d riprel.abs` | listings | RIP-relative vs absolute immediate |

## How to navigate the listings

1. Search `<main>:` in both listings.
2. In the PIE build, find `lea …(%rip)` toward `msg` — decode with the column
   table above.
3. In the absolute/`-no-pie` build, find an immediate load of `msg`'s address —
   note the absence of `(%rip)`.
4. On the `call` toward `puts`, note `@plt` if present — different mechanism.
5. Optional: `objdump -r` on the `.o` files to see relocation *names* side by
   side before link.

## Worked example

**The situation.** The stub passes NULL to `puts` and crashes until you compute
`msg`'s address correctly.

**Step 1 — RIP-relative load.** Use `lea msg(%rip), %rdi` (or equivalent), then
call `puts`. `make && make run` should print the message; `readelf -h` should
show type `DYN` for the PIE.

**Step 2 — force the absolute mistake.** `make absolute` and read the linker
error's relocation name. Then `make absolute-nopie && ./riprel.abs` to see the
same absolute object work as `EXEC`.

**Step 3 — compare listings.** `make disasm` vs `objdump -d riprel.abs` on
`main`: one shows RIP-relative, the other an absolute immediate. On the RIP
line, name instruction address vs displacement vs `#` target out loud.

**Step 4 — optional ASLR contrast.** Run the PIE twice (`setarch -R ./riprel`
vs plain if available). Behavior should persist while load base can move —
exactly why RIP-relative was required.

**Rejected wrong reading:** "Forgetting `lea` still links but somehow works."
Forgetting it often links (NULL in `%rdi`) and crashes in `puts`; the absolute
form often **never links** as PIE. Two different failure modes — do not confuse
them.

## Distinctions worth keeping straight

- **PIE `DYN` vs non-PIE `EXEC`** — `readelf -h` Type field.
- **RIP-relative data address vs PLT call** — both are "position independent,"
  different mechanisms.
- **Link-time relocation failure vs runtime NULL crash** — absolute form often
  never links as PIE; forgetting `lea` links but crashes in `puts`.
- **Disassembly comment address vs runtime ASLR address** — comments are for
  that linked image layout.
- **Instruction address vs `%rip` displacement vs `#` target** — same lesson-01
  trap in a data-address costume.

## Check yourself

1. Which relocation shows up for `movl $msg, %edi` versus the RIP-relative
   form? (`objdump -r` on the objects.)
2. Why can a 32-bit absolute relocation not be fixed at load for a PIE the way
   RIP-relative needs no absolute fixup?
3. Is `%rip` the start or the end of the current instruction when the
   displacement applies?
4. Why does `call puts` not need the same `lea` treatment as `msg`?
5. On a `lea 0xe61(%rip),%rdi # 2010 <msg>` line, which token is the instruction
   address, which is the displacement, and which is the resolved target?
6. What `readelf -h` field distinguishes your PIE from `riprel.abs`?

## Key takeaways

- PIE asm addresses static data with `thing(%rip)`, not `$thing` immediates.
- Absolute 32-bit relocations fail at link for PIEs; `-no-pie` is a different
  (legacy) contract.
- RIP-relative uses the next instruction's address plus a displacement — decode
  the `objdump` columns; do not treat every hex token as an address.
- Libc calls usually go via PLT; that is not the same as materializing a data
  address.
- Link failure and NULL crash are different wrong readings — name which you
  hit.


## `readelf -h` Type field

| Type | Meaning in this exercise |
| ---- | ------------------------ |
| `DYN` | PIE (or shared object) — default modern executable |
| `EXEC` | traditional non-PIE executable — what `absolute-nopie` aims for |

Search the header dump for the literal `Type:` line. Matching that field to
which `make` target you used is the recognition check before you argue about
ASLR.

## Lookup (not the lesson)

- `info as` (node: i386-Memory)
- `man 1 ld`, `man 1 objdump`, `man 1 readelf`
- `man 1 gcc` (`-no-pie`, `-fPIE`), `man 8 setarch`, `man 3 puts`

Now open `TASK.md` and do the practice.
