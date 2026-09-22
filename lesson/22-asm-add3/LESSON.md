# 22 — Hand-written `add3` under the C ABI

C and assembly meet at a calling convention: where arguments land, where the
return value must appear, and which registers you may freely overwrite. This
lesson makes you implement a three-argument integer function the way System V
AMD64 expects, so a C harness can call your `.s` without glue. By the end you
should name the three inbound registers, point at `%rax` as the return slot, and
recognize a leaf `add3` in `objdump` without inventing a mandatory frame.

## What this lesson asks of you

Implement `long add3(long a, long b, long c)` in GNU as AT&T so the sum returns
in `%rax`. You should already have seen the register story from Track A
(arguments and returns); here you *author* it. No callee-saved traffic is
required if you only touch caller-saved registers.

`TASK.md` is the practice. Man pages spell flags; they do not teach the ABI.

## Integer argument and return registers

For the System V AMD64 ABI used on Linux x86-64, the first integer/pointer
arguments arrive in fixed registers. Decode the call boundary as a table:

| Role | Register | For `add3(a, b, c)` |
| ---- | -------- | ------------------- |
| Arg 1 | `%rdi` | `a` |
| Arg 2 | `%rsi` | `b` |
| Arg 3 | `%rdx` | `c` |
| Arg 4–6 | `%rcx`, `%r8`, `%r9` | unused here |
| Integer return | `%rax` | must hold `a+b+c` at `ret` |

So on entry to your label, the three values are *already* in those registers —
the C harness's `call add3` put them there. Your job is arithmetic and a clean
`ret`, not "reading arguments from the stack" the way i386 often did.

You discover this from `gcc -S` on a tiny C `add3`, not from an ABI PDF. The
compiler's `.s` is the local ground truth this track trusts.

**Rejected wrong reading:** "Arguments are on the stack; I should load
`8(%rsp)` / `16(%rsp)`." On this ABI the first six integer args are in
registers. Stack slots appear for args 7+ or for spilled locals — not for
`add3`'s three `long`s at the call boundary.

## Caller-saved versus callee-saved (just enough)

| Class | Registers (common set) | Rule |
| ----- | ---------------------- | ---- |
| Caller-saved (scratch) | `%rax`, `%rcx`, `%rdx`, `%rsi`, `%rdi`, `%r8`–`%r11` | Callee may overwrite freely |
| Callee-saved | `%rbx`, `%rbp`, `%r12`–`%r15` | If you use them, save before / restore before `ret` |

If you only use scratch registers, you can compute and `ret` with no
`push`/`pop`. For a three-register add you should not need callee-saved regs.

## Shape of a minimal AT&T function

A global function needs:

1. A visible symbol: `.globl add3` (or `.global add3`).
2. A label `add3:` (note the colon — this is the assembly name the linker sees).
3. Instructions that honor the register contract.
4. `ret` — returns to the harness; you are **not** writing `_start`.

You are not freestanding; the C harness and CRT still own process startup. Your
job is only the callee side of one call.

AT&T syntax writes source then destination: `addq %rsi, %rdi` adds `%rsi` into
`%rdi`. Width suffixes (`q` for 64-bit) match the `long` in the C prototype on
this LP64 platform. Immediates take `$`; registers take `%`.

### What `make` maps to

| You type | What happens | Open afterward |
| -------- | ------------ | -------------- |
| `make` | assemble `add3.s`, link with harness | `./add3` (name may vary) |
| `make run` | run harness checks | stdout `ok` or `FAIL` |
| `make asm` / `gcc -S` on a C reference | compiler's idea of `add3` | reference `.s` |
| `objdump -d` on the binary | machine view of your symbol | search `<add3>:` |

## How to find your function in a listing

After a successful build:

1. Run `objdump -d` on the linked harness binary (or the `.o` if you prefer).
2. **Search for `<add3>:`** — angle brackets and colon. That is your symbol.
3. Treat CRT, `main` in the harness, and PLT stubs as someone else's code.
4. Inside `<add3>:`, expect a short chain of `add` / `lea` / `mov` ending in
   `ret`. At `-O0` from GCC you might also see a frame; your hand-written leaf
   may omit it.

A plausible shape (immediates/addresses vary; columns do not):

```text
0000000000001169 <add3>:
    1169:	48 89 f8             	mov    %rdi,%rax
    116c:	48 01 f0             	add    %rsi,%rax
    116f:	48 01 d0             	add    %rdx,%rax
    1172:	c3                   	ret
```

| Column | Example | Meaning |
| ------ | ------- | ------- |
| Address | `1169:` | Where this instruction lives |
| Bytes | `48 89 f8` | Machine encoding |
| Mnemonic | `mov %rdi,%rax` | Copy `a` into the return register |
| Later adds | `add %rsi,%rax` etc. | Fold in `b` and `c` |

**Rejected wrong reading:** "The sum can stay in `%rdi` because that is where
`a` started." The C caller reads `%rax` after `ret`. Leaving the total only in
`%rdi` is an ABI bug that sometimes passes one test by luck and fails the next.

**Rejected wrong reading:** "I must `push %rbp` and build a frame or it is not a
real function." A leaf that needs no locals may skip a frame; the ABI does not
require a frame pointer. Frames help debugging and locals — they are not a tax
on every callee.

## Worked example

**The situation.** The stub `add3.s` exports `add3` but only `ret`s, so the
harness sees a garbage return and prints `FAIL`.

**Step 1 — name the registers from a C reference.** Compile a one-line C
`add3` with `gcc -S -O0` (or reopen exercise 04–05 artifacts). Search the `.s`
for `add3:`. Confirm the three incoming registers and that the arithmetic ends
with a value in `%rax`. The rejected wrong reading is inventing stack argument
offsets from an old i386 tutorial.

**Step 2 — implement only the arithmetic.** One honest plan: `mov %rdi, %rax`,
then `add %rsi, %rax`, then `add %rdx, %rax`, then `ret`. Other orders work if
`%rax` holds the sum at the end. The rejected wrong reading is adding into
`%rdi` and returning without moving to `%rax`.

**Step 3 — run the harness.** `make && make run` expects cases like
`add3(1,2,3) == 6`. If you swapped two argument registers, one case may pass by
luck while another fails — read failures as ABI mistakes, not "math bugs."

**Step 4 — confirm in disassembly.** Search `<add3>:`. Check that you do not
see a `syscall` (wrong exercise) and that `ret` is present. Compare column
layout to the table above so you do not confuse addresses with immediates.

## Distinctions worth keeping straight

- **C prototype vs asm symbol** — same name, same register contract; the
  assembler does not check types.
- **Return in `%rax` vs "the last register I added into"** — if you left the
  sum only in `%rdi`, C still reads `%rax`.
- **Leaf function vs framed function** — frames are for locals and debug
  convenience; they are not mandatory for every callee.
- **Your `.s` vs freestanding `_start`** — this exercise still links with CRT
  and a C `main` in the harness.
- **Caller-saved vs callee-saved** — scratch is free to trash; `%rbx` and friends
  need save/restore if touched.

## Check yourself

1. In which three registers do `a`, `b`, and `c` arrive?
2. Where must the sum be when `ret` executes?
3. If you used `%rbx` as a temporary, what else must you do before returning?
4. Why can this function omit `push %rbp` and still be ABI-correct?
5. In an `objdump -d` listing, what exact search string takes you to your code
   rather than the harness `main`?
6. On a `mov %rdi,%rax` line, which token is the instruction address?

If any answer is fuzzy, re-read above before opening `man as` for directive
spellings.

## Key takeaways

- Linux x86-64 C calls pass early integer args in `%rdi`, `%rsi`, `%rdx`, … and
  take integer returns from `%rax`.
- A hand-written asm function is just a global label that respects that
  contract.
- Prefer caller-saved scratch for tiny leaves; preserve callee-saved if you
  touch them.
- Validate against `gcc -S` output and a harness, not against folklore.
- Find your work with `<add3>:`; do not drown in CRT.

## Lookup (not the lesson)

- `man 1 as`, `man 1 gcc`, `info as`
- Discover registers from your own Track A `gcc -S` artifacts (ex. 04–05)
- `man 1 objdump` — when you need listing flag spellings

Now open `TASK.md` and do the practice.
