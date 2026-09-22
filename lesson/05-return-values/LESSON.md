# 05 — Return values

Arguments enter a function through a convention; return values leave through one
too. For ordinary `int` and `long` results on SysV AMD64, the callee places the
value in a return register before `ret`, and the caller reads it. Small structs
may return in registers or via a *hidden pointer* depending on size and layout —
and the truthful source for which strategy GCC chose is again your `.s` file.
By the end of this lesson you should spot both scalar returns and a small-struct
return path in caller and callee listings.

## What this lesson asks of you

Implement `ret_int`, `ret_long`, and `ret_pair` returning a two-`int` struct.
Call them from `main`, print results, and read `-O0` assembly on both sides of
each `call`. Practice is in `TASK.md`.

## What `make` is naming for you

| You type | Open afterward |
| -------- | -------------- |
| `make asm` | `prog.s` — end of each callee + call sites in `main` |
| `make disasm` | `prog.lst` — search `<ret_int>:`, `<ret_pair>:`, `<main>:` |

## Scalar returns

For `int` / `long` (and other small integer-class results), watch the end of the
callee: a move into `%eax` or `%rax` followed by epilogue + `ret` is the usual
shape.

Example shapes (comments abbreviated):

```text
ret_int:
        … 
        movl    $42, %eax
        popq    %rbp
        ret

ret_long:
        …
        movabsq $0x1122334455667788, %rax
        popq    %rbp
        ret
```

| Width | Typical return register form | What to look for in the caller |
| ----- | ---------------------------- | ------------------------------ |
| `int` (32-bit) | `%eax` via `movl` | `movl %eax, …` or use of `%eax` after `call` |
| `long` (64-bit) | `%rax` via `movq` / `movabsq` | 64-bit use of `%rax` after `call` |

### Navigation

1. Search `ret_int:` / `ret_long:` in `prog.s`.
2. Scroll to the last moves before `pop` / `leave` / `ret`.
3. Note the register that holds the distinctive constant you returned.
4. Search `call ret_int` in `main` and read the *next* few instructions — that is
   where the caller consumes the return register.

**Rejected wrong reading:** "the return value is whatever is in `%rbp`." Frame
pointers are not return channels. Prologue/epilogue traffic around `%rbp` is
housekeeping (lesson 02).

## Small struct returns

A `struct Pair { int x; int y; };` is eight bytes with typical alignment. GCC may:

- Return the two fields packed into one or more return registers (often both
  fields in `%rax` as an 8-byte blob), or
- Pass a hidden pointer to caller-allocated memory as an invisible first
  argument; the callee writes fields through that pointer.

You decide which happened by reading **both** sides.

### Callee evidence (register-return shape)

A common `-O0` pattern for an 8-byte pair:

```text
ret_pair:
        …
        movl    $3, -8(%rbp)    # p.x
        movl    $5, -4(%rbp)    # p.y
        movq    -8(%rbp), %rax  # pack both ints into %rax
        popq    %rbp
        ret
```

| Clue | Reading |
| ---- | ------- |
| Fields built in a local slot | Ordinary locals. |
| Final `movq …, %rax` then `ret` | Whole struct rides home in `%rax`. |
| No store through a pointer arg | Not the hidden-pointer path *for this build*. |

### Caller evidence

After `call ret_pair`, the caller may:

- Keep `%rax` and unpack with shifts / `movd` / stores into a `struct Pair`
  local, or
- Have passed a buffer address in `%rdi` before the call (hidden pointer) — then
  you would also see the callee writing through `%rdi` / a spill of that pointer
  instead of returning fields in `%rax`.

On many current GCC builds for `{int;int}`, you will see the **register** path.
If your listing shows a hidden pointer instead, believe *your* listing — that is
the point of the exercise.

**Rejected wrong reading:** "structs always return on the stack" or "structs
always return in `%rax`." Size and ABI classification decide; your listing shows
the choice for *this* struct on *this* compiler.

**Rejected wrong reading:** "printing `p.x` in `main` means the struct was
returned field-by-field as two separate C returns." It is still one return;
printing just reads members after the aggregate arrived.

## Large structs (preview)

Once a struct no longer fits the "small enough" classification, returns
reliably go through memory via a hidden pointer — effectively an extra first
parameter. Your `Pair` of two ints is deliberately on the borderline where you
must *look*. Huge structs are less ambiguous but also less educational for this
exercise.

## Worked example

**The situation.** `ret_int` returns `42`, `ret_long` returns a distinctive
64-bit constant, `ret_pair` returns `{3, 5}`. `main` prints all three.

**Step 1 — `ret_int` / `ret_long`.** At each `ret`, note the return register.
Confirm `main` consumes the same register after `call`.

**Step 2 — `ret_pair` callee.** See whether the function writes `%rax` (or
`%rax`/`%rdx`) or stores through a pointer. Cite the concrete instructions.

**Step 3 — `ret_pair` caller.** Match the callee story: hidden pointer setup
versus register unpack into locals. Sketch a one-sentence data path from callee
to `printf`.

**Step 4 — sanity-check by changing field values.** Rebuild and ensure the new
constants appear in the same channels. If the channel moves, you misidentified
setup noise as the return path.


## Hidden-pointer return — what evidence looks like

If GCC *did* pass a hidden pointer for `ret_pair`, expect something in this
family:

**Caller:** allocate a `Pair` slot; put its address in `%rdi` before `call`;
maybe shuffle other args over by one.

**Callee:** first incoming pointer treated as "where to write the result";
stores like `movl $3, (%rdi)` / `movl $5, 4(%rdi)`; may return that pointer in
`%rax` as well (implementation texture).

If you instead see the register-pack path from earlier (`movq` of both ints into
`%rax`), you do **not** need to invent a hidden pointer. Cite what you have.

### Width checklist for scalars

| C return type | Likely move into return reg | Caller consume |
| ------------- | --------------------------- | -------------- |
| `int` | `movl $…, %eax` | 32-bit use of `%eax` |
| `long` | `movq` / `movabsq` into `%rax` | 64-bit use of `%rax` |
| `struct Pair` (8 bytes, register path) | `movq` blob into `%rax` | unpack / store to local `Pair` |

## Distinctions worth keeping straight

- **Return register vs argument registers** — opposite directions.
- **Scalar vs aggregate return** — aggregates may use a hidden pointer.
- **Caller preparation vs callee writeback** — both halves define the story.
- **C source return vs machine channel** — one abstract value, concrete homes.
- **`%rbp` epilogue vs `%rax` return** — do not conflate them.

## Check yourself

1. Where do you look in the callee to find an `int` return at `-O0`?
2. Why must you read the caller when diagnosing a struct return?
3. What evidence suggests a hidden pointer was passed for `ret_pair`?
4. How would a wrong reading confuse `%rbp` prologue traffic with returning?
5. If `ret_pair` ends with `movq -8(%rbp), %rax`, what became of `x` and `y`?
6. What search string finds the callee's return setup quickly in `prog.s`?

## Key takeaways

- Integer-class scalars return in `%rax`/`%eax`.
- Small structs may use registers or a hidden pointer; size/layout decide —
  confirm on both sides of the `call`.
- Distinctive constants make the channel obvious; change them to re-verify.
- Discover the channel from `gcc -S`, not from folklore.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, `-O`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
