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

## Scalar returns

For `int` / `long` (and other small integer-class results), watch the end of the
callee: a move into `%eax` or `%rax` followed by epilogue + `ret` is the usual
shape. The caller then uses that register — copy it to a local, pass it to
`printf`, or compare it.

Widths matter visually: 32-bit returns often show `movl` into `%eax`; 64-bit
returns show `movq` / `%rax`. Upper bits can be extension noise; focus on where
the *value you returned* is placed.

## Small struct returns

A `struct Pair { int x; int y; };` is eight bytes with typical alignment. GCC may:

- Return the two fields packed into one or more return registers, or
- Pass a hidden pointer to caller-allocated memory as an invisible first
  argument; the callee writes fields through that pointer.

You decide which happened by reading **both** sides:

- In the **caller**, look for a stack slot or object address prepared before
  `call`, and whether an extra argument register is loaded with that address.
- In the **callee**, look for stores through a pointer argument versus moves
  into return registers at the end.

The rejected wrong reading is: "structs always return on the stack" or "structs
always return in `%rax`." Size and ABI classification decide; your listing
shows the choice for *this* struct on *this* compiler.

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
Confirm `main` consumes the same register after `call`. The rejected wrong
reading is: "the return value is whatever is in `%rbp`." Frame pointers are not
return channels.

**Step 2 — `ret_pair` callee.** See whether the function writes `%rax`/`%rdx`
(or similar) or stores through a pointer. The rejected wrong reading is:
"printing `p.x` in `main` means the struct was returned field-by-field as two
separate C returns." It is still one return; printing just reads members.

**Step 3 — `ret_pair` caller.** Match the callee story: hidden pointer setup
versus register unpack into locals. Sketch a one-sentence data path from callee
to `printf`.

**Step 4 — sanity-check by changing field values.** Rebuild and ensure the new
constants appear in the same channels. If the channel moves, you misidentified
setup noise as the return path.

## Distinctions worth keeping straight

- **Return register vs argument registers** — opposite directions.
- **Scalar vs aggregate return** — aggregates may use a hidden pointer.
- **Caller preparation vs callee writeback** — both halves define the story.
- **C source return vs machine channel** — one abstract value, concrete homes.

## Check yourself

1. Where do you look in the callee to find an `int` return at `-O0`?
2. Why must you read the caller when diagnosing a struct return?
3. What evidence suggests a hidden pointer was passed for `ret_pair`?
4. How would a wrong reading confuse `%rbp` prologue traffic with returning?

## Key takeaways

- Integer-class scalars return in a dedicated return register (`%rax`/`%eax`
  class).
- Small structs may use registers or a hidden pointer; size/layout decide.
- Always correlate caller and callee listings for aggregates.
- Discover the channel from `gcc -S`, not from folklore.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`, `-O`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
