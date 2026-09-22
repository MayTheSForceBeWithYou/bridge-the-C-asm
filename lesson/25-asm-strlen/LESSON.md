# 25 — Walking bytes until NUL

A C string is a pointer to bytes terminated by `\0`. Computing its length is a
pointer walk: load a byte, test for zero, advance, count. This lesson has you
write that walk in asm without calling libc `strlen`, so addressing modes and
byte-sized ops become concrete. By the end you should decode `(%rdi)` as "byte
at the address in `%rdi`," keep pointer math in quads, and treat the empty
string as length 0.

## What this lesson asks of you

Implement `long my_strlen(const char *s)` returning the count of bytes before
the first NUL. The pointer is non-NULL in tests. Pure asm in `my_strlen.s`; do
not `call strlen`.

## Bytes versus quads

The argument `s` arrives in `%rdi` as a 64-bit pointer. The *characters* are
8-bit values. Mixing those widths is the main failure mode.

| What | Size | Typical op |
| ---- | ---- | ---------- |
| Pointer `s` | 64-bit | `%rdi`, `incq %rdi` |
| One character | 8-bit | `movb (%rdi), %al` or `movzbl (%rdi), %eax` |
| Length counter | 64-bit return | accumulate in `%rax` (or move there before `ret`) |

Using `movq (%rdi), %rax` would pull eight characters at once — wrong for a
simple NUL scan unless you write a wider SIMD-style loop (out of scope).

## Addressing modes you will see

AT&T memory operands put the address expression in parentheses. Decode the
common forms:

| Form | Example | Meaning |
| ---- | ------- | ------- |
| Indirect | `(%rdi)` | memory at address held in `%rdi` |
| Base+index | `(%rbx,%rcx)` | memory at `%rbx + %rcx` |
| Base+index*scale | `(%rbx,%rcx,1)` | same with scale 1 (byte index) |
| Displacement | `5(%rdi)` | memory at `%rdi + 5` (rarely needed for forward scans) |

**Recognition rule:** the thing outside the parentheses on a `mov` is the
register/data; the parenthesized expression is the *address*. `movb (%rdi), %al`
loads a byte from memory into `%al`. It does not "move the pointer into `%al`."

A listing line:

```text
    1142:	0f b6 07             	movzbl (%rdi),%eax
    1145:	84 c0                	test   %al,%al
    1147:	74 0c                	je     1155 <my_strlen+0x1d>
```

| Column / token | Example | Meaning |
| -------------- | ------- | ------- |
| Address | `1142:` | Where the load lives |
| Mnemonic | `movzbl (%rdi),%eax` | zero-extend byte at `*%rdi` into `%eax` |
| `(%rdi)` | address expression | *where* to load from |
| `test %al,%al` | flag set for zero-byte | prepares `je` |
| `je 1155 <…>` | target | exit when NUL seen; `1147:` is the jump's own address |

**Rejected wrong reading:** "`movb %rdi, %al` is how I read the first
character." That truncates the *pointer value* into 8 bits. You need a memory
operand: `(%rdi)`.

**Rejected wrong reading:** "`74 0c` is the string length." Those are the jump's
machine bytes (opcode + displacement).

GAS local labels (`.Lloop`, `.Ldone`) keep the symbol table clean: only
`my_strlen` needs to be global for the harness.

## The load / test / branch / advance loop

A clear shape:

1. Zero a counter (`xor %rax, %rax`) if you count up from zero.
2. Load one byte: `movb (%rdi), %al` (or `movzbl` into `%eax`).
3. Test for NUL: `testb %al, %al` (or `cmpb $0, %al`) then `je .Ldone`.
4. On non-zero: increment counter, advance pointer (`incq %rdi`), `jmp` back.

The returned length excludes the NUL, matching libc `strlen`.

### Empty string is the recognition test

For `s` pointing at `""`, the first byte load is already `0`. You must return 0
*without* incrementing. If your loop increments before testing, you report 1 —
the classic off-by-one.

**Rejected wrong reading:** "Increment first, then test." That reports 1 for an
empty string.

**Rejected wrong reading:** "Count the NUL too." Length is the offset of the
terminator, not including it.

## What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | harness compares against known lengths |
| `objdump -d` | search `<my_strlen>:`; look for `movzbl`/`movb` and `test`/`jcc` |
| Forbidden | `call strlen` / `call my_strlen@plt` to libc |

## How to navigate the listing

1. Search `<my_strlen>:`.
2. Inside it, find a byte load (`movb` or `movzbl`) — that is the character
   fetch. Confirm the memory operand uses your pointer register.
3. Find the `test`/`cmp` on that byte and the `jcc` to the exit path.
4. Confirm `ret` leaves the count in `%rax`.
5. Confirm there is no `call …strlen@plt` — that would be cheating the exercise.

## Worked example

**The situation.** The harness passes several C string literals and compares
your return value to the known length.

**Step 1 — empty string.** `my_strlen("")` must return 0: first byte load is
already zero, counter untouched. Sketch that path before the happy path.

**Step 2 — single byte.** For `"a"`, load `'a'`, branch to continue, bump count
to 1, advance, load `0`, exit with `%rax == 1`.

**Step 3 — watch width.** Prefer consistent `q`-suffix pointer math and a clear
64-bit count in `%rax` on `ret`. Mixing `movl` into a pointer register without
zero-extending creates bugs that show up on longer strings or high addresses.

**Step 4 — prove you did not call libc.** In `objdump -d`, inside `<my_strlen>:`,
you should see your loop — not `call … <strlen@plt>`. Linkage might resolve
`strlen`, but the exercise forbids it so you own the loads.

## Distinctions worth keeping straight

- **Pointer in `%rdi` vs byte in `%al`** — different sizes, same string.
- **`movzbl` vs `movb` into `%al`** — zero-extending into `%eax` clears the
  upper bits; sometimes cleaner before wider ops.
- **Address `(%rdi)` vs value `%rdi`** — parentheses mean memory.
- **Instruction address vs jump target** — left column vs `je`'s operand.
- **Your loop vs `repnz scasb`** — micro-optimizations are optional; clarity
  first.
- **Not calling libc** — owning the loads is the point.

## Check yourself

1. Which register holds `s` on entry?
2. How do you test one byte for NUL without treating it as an 8-byte integer?
3. What should `my_strlen("")` return, and which loop order guarantees that?
4. In `movb (%rdi), %al`, which token is the address expression?
5. On a `je 1155 <my_strlen+0x1d>` line, what is the instruction address of the
   jump?
6. Where must the count be when you `ret`?
7. What search string finds your function rather than harness code?

## Key takeaways

- `strlen`-like code is load-byte / test / branch / advance / count.
- Use byte ops (`b`) on character data; keep the pointer as a quad.
- Decode `(%reg)` as a memory address — that is the recognition rule.
- Empty string and "count excludes NUL" are the correctness anchors.
- ABI return still means `%rax`, even for a "simple" loop.


## Index style versus pointer bump

Two equivalent memory stories:

| Style | Idea | Typical load |
| ----- | ---- | ------------ |
| Bump pointer | `%rdi` advances each step | `movb (%rdi), %al` then `incq %rdi` |
| Fixed base + index | `%rdi` stays; `%rcx` grows | `movb (%rdi,%rcx,1), %al` then `incq %rcx` |

Pick one and keep it for the whole function. Mixing them — advancing `%rdi`
*and* using an index that also grows — double-counts and returns a length that
is nonsense. In the listing, you should be able to point at a single induction
variable.

When you use the index style, the length on exit is often already in the index
register; `mov` it into `%rax` if needed. When you bump the pointer, the counter
is separate — do not return the pointer value as if it were a length.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Do not call libc `strlen` for the implementation
- `man 1 objdump` — to verify your loop shape

Now open `TASK.md` and do the practice.
