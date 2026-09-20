# 25 — Walking bytes until NUL
<!-- concepts: c.p2.strings.nul, c.p5.asm.loop -->

A C string is a pointer to bytes terminated by `\0`. Computing its length is a
pointer walk: load a byte, test for zero, advance, count. This lesson has you
write that walk in asm without calling libc `strlen`, so addressing modes and
byte-sized ops become concrete.

## What this lesson asks of you

Implement `long my_strlen(const char *s)` returning the count of bytes before
the first NUL. The pointer is non-NULL in tests. Pure asm in `my_strlen.s`; do
not `call strlen`.

## Bytes versus quads

## Addressing modes you will see

AT&T memory operands put the address in parentheses: `(%rdi)` is "byte at the
address held in `%rdi`." A displacement form `-1(%rdi)` is rarely needed for
forward scans; an indexed form `(%rbx,%rcx,1)` keeps a fixed base and a growing
index. Pick one style and stick to it inside the function so you do not lose the
pointer while counting.

GAS local labels (`.Lloop`, `.Ldone`) keep the symbol table clean: only
`my_strlen` needs to be global for the harness.


The argument `s` arrives in `%rdi` as a 64-bit pointer. The *characters* are
8-bit values. Loading with `movb (%rdi), %al` (or `movzbl (%rdi), %eax`) reads
one byte. Using `movq (%rdi), %rax` would pull eight characters at once — wrong
for a simple NUL scan unless you write a wider SIMD-style loop (out of scope).

`testb %al, %al` (or `cmpb $0, %al`) sets flags so you can `je` to the exit when
the byte is zero. Each non-zero byte increments a counter (often `%rax` from the
start, or a separate register you later move into `%rax`).

## Advancing the pointer

After a non-zero byte, add 1 to the pointer (`incq %rdi` or `addq $1, %rdi`) and
jump back to the load. You may instead keep a base in one register and an index
in another (`movb (%rdi,%rcx,1), %al`) — same machine idea: address = base +
offset.

Do not write past the terminator; you only read until NUL. The returned length
excludes the NUL, matching libc `strlen`.

## Worked example

**The situation.** The harness passes several C string literals and compares
your return value to the known length.

**Step 1 — empty string.** `my_strlen("")` must return 0: first byte load is
already zero, counter untouched. The rejected wrong reading is: "increment
first, then test," which reports 1 for an empty string.

**Step 2 — single byte.** For `"a"`, load `'a'`, branch to continue, bump count
to 1, advance, load `0`, exit with `%rax == 1`. The rejected wrong reading is:
"count the NUL too." Length is the offset of the terminator, not including it.

**Step 3 — watch width.** If you accidentally `incq` a 32-bit-only counter
story or mix `movl` into a pointer register without zeroing, long strings or
high addresses misbehave. Prefer consistent `q`-suffix pointer math and a clear
64-bit count in `%rax` on `ret`.

## Distinctions worth keeping straight

- **Pointer in `%rdi` vs byte in `%al`** — different sizes, same string.
- **`movzbl` vs `movb` into `%al`** — zero-extending into `%eax` clears the
  upper bits; sometimes cleaner before wider ops.
- **Your loop vs `repnz scasb`** — micro-optimizations are optional; clarity
  first.
- **Not calling libc** — linkage might resolve `strlen`, but the exercise forbids
  it so you own the loads.

## Check yourself

1. Which register holds `s` on entry?
2. How do you test one byte for NUL without treating it as an 8-byte integer?
3. What should `my_strlen("")` return?
4. Where must the count be when you `ret`?

## Key takeaways

- `strlen`-like code is load-byte / test / branch / advance / count.
- Use byte ops (`b`) on character data; keep the pointer as a quad.
- Empty string and "count excludes NUL" are the correctness anchors.
- ABI return still means `%rax`, even for a "simple" loop.

## Lookup (not the lesson)

- `man 1 as`, `info as`
- Do not call libc `strlen` for the implementation

Now open `TASK.md` and do the practice.
