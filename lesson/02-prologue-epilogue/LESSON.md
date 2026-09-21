# 02 — Prologue / epilogue

Every C function you write becomes a labeled region of machine code with an entry
and an exit. At `-O0`, GCC almost always wraps that region in a *prologue* that
sets up a frame pointer and an *epilogue* that tears it down before `ret`. By the
end of this lesson you should recognize those bookends on sight, say what
`%rbp` / `%rsp` are doing during them, and predict how `-O2` can shrink or erase
an empty function entirely.

## What this lesson asks of you

You will read compiler-generated assembly for a nearly empty function and for
`main`, at two optimization levels. The competence is not memorizing opcodes; it
is knowing which instructions are "housekeeping" so later lessons can ignore them
and focus on the interesting middle. Practice lives in `TASK.md`. Man pages are
for flag spellings once you already know what you need.

## Frame pointer setup at `-O0`

On x86-64 System V (the ABI GNU/Linux uses here), a typical unoptimized function
opens like this:

1. `push %rbp` — save the caller's frame pointer on the stack, and shrink `%rsp`
   by 8.
2. `mov %rsp, %rbp` — make the current stack pointer the *frame pointer* for this
   function. From now on, locals and spilled values are addressed as offsets from
   `%rbp` (for example `-4(%rbp)`).
3. Optionally `sub $N, %rsp` — reserve `N` bytes of stack for locals / alignment.

The matching close is the reverse: restore `%rsp` from `%rbp` (or `leave`),
`pop %rbp`, then `ret`. Together those bookends are the prologue and epilogue.
They exist so every local has a stable address relative to `%rbp` even if `%rsp`
moves for other reasons (pushes, `alloca`, further calls).

With `-fverbose-asm` (this track's default for `make asm`), GCC plants comments
naming the C entities next to instructions. Use those comments to confirm that
the `push` / `mov` pair is frame setup, not "your" logic.

## What "empty" still costs

A C function `void empty(void) {}` has no statements, but at `-O0` it is still a
real call target: prologue, epilogue, `ret`. Calling it from `main` still emits a
`call empty`. The rejected wrong reading people make here is "empty means zero
bytes." Empty means zero *payload*; the ABI still wants a callable symbol unless
the optimizer proves the call is useless.

`main` usually looks heavier: it may align the stack (SysV wants `%rsp` 16-byte
aligned before a `call`), set up locals, and arrange a return value in `%eax`.
Compare `empty` to `main` side by side so you separate "frame ritual" from
"program work."

## What `-O2` is allowed to do

Optimization can:

- **Inline** a tiny callee into its caller, so the `call` / `ret` pair vanishes.
- **Eliminate** a function that is never needed after inlining, so the symbol may
  disappear from the `.s` / binary you inspect.
- **Omit the frame pointer** (`-fomit-frame-pointer` is common at `-O2`), addressing
  locals from `%rsp` instead of `%rbp`, which shortens or removes the classic
  `push %rbp` / `mov %rsp,%rbp` pair.

So when you rebuild with `make O=2 asm`, do not expect the `-O0` shape. Expect a
different contract: the compiler still implements your C semantics, but the
housekeeping may be gone.

## Worked example

**The situation.** You implement `void empty(void) {}`, call it once from `main`,
and emit assembly at `-O0` and `-O2`.

**Step 1 — open the `-O0` `.s` and find `empty`.** You should see a label, a
short prologue, and an epilogue ending in `ret`. The rejected wrong reading is:
"`push %rbp` is pushing my function's return value." No — it saves the *caller's*
`%rbp`. Your return value (if any) would be in a return register at `ret` time;
`empty` returns void, so there is nothing to hand back.

**Step 2 — find `main`'s call site.** Look for `call empty` (or a decorated
symbol form). Note any stack alignment around the call. The rejected wrong
reading is: "the `sub $N,%rsp` in `main` is the size of `empty`." Frame size in
`main` is about `main`'s own needs (alignment, locals), not the callee's body.

**Step 3 — rebuild at `-O2` and compare.** Check whether `empty` still appears as
a standalone function, whether `main` still contains a `call`, and whether frame
pointer instructions remain. The rejected wrong reading is: "`-O2` failed because
my empty function vanished." Vanishing can be correct optimization when nothing
observable remains.

**Step 4 — skim `make disasm` listings.** Confirm the same story in `objdump -d`
output: labels, `call` targets, and whether optimized builds still have a
distinct `empty` symbol.

## Distinctions worth keeping straight

- **Prologue/epilogue vs your statements** — bookends versus payload.
- **`%rbp` frame pointer vs `%rsp` stack pointer** — stable local base versus
  moving top of stack.
- **`-O0` pedagogy shape vs `-O2` real shape** — both are valid; they answer
  different questions.
- **Inlining vs deletion** — inlining copies the body into the caller;
  elimination drops a symbol that nothing needs.

## Check yourself

Close this file and answer from memory:

1. What two instructions typically open an `-O0` function that uses a frame
   pointer, and what does each accomplish?
2. Why does `void empty(void) {}` still generate machine code at `-O0`?
3. Name two ways `-O2` might change what you see for `empty` and its call site.
4. If `%rsp` changes during a function, why did classical frames bother setting
   `%rbp` at entry?

If an answer is fuzzy, re-read the matching section — do not open `man gcc` yet.
When you need the exact spelling of `-fverbose-asm` or `-O2`, *then* look it up.

## Key takeaways

- At `-O0`, expect a visible prologue/epilogue around almost every function.
- `push %rbp` / `mov %rsp,%rbp` establish a frame; epilogue undoes it before `ret`.
- Optimization may omit frame pointers, inline callees, or delete dead functions.
- You discover the shape from `gcc -S` / `objdump -d`, not from an ABI PDF.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- `info gcc` — optimization overview after practice

Now open `TASK.md` and do the practice.
