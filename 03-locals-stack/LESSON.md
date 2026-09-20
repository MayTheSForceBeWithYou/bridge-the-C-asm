# 03 — Locals on the stack

C local variables are names in your source. In unoptimized x86-64 code they are
usually *slots* at negative offsets from `%rbp`. Optimized code may keep the same
values only in registers — or delete them if nothing observable uses them. By the
end of this lesson you should read `-N(%rbp)` as "a local," estimate frame size
from `sub $N,%rsp`, and expect `-O2` to rearrange the story.

## What this lesson asks of you

Declare several locals of mixed types, force them to matter (print, take
addresses, or combine into a return), and compare `-O0` versus `-O2` assembly.
The skill is mapping C names to homes in the machine, not drawing pretty stack
diagrams from memory. `TASK.md` is the practice; this file teaches what to look
for.

## Stack slots at `-O0`

With a frame pointer in place (lesson 02), GCC typically assigns each live local
an offset below `%rbp`. An `int` often occupies 4 bytes; a `long` on LP64
occupies 8; a `char` occupies 1 but may still sit at an aligned offset with
padding around it. An array of four `int`s is sixteen bytes of consecutive slots.

Stores look like `movl $1, -4(%rbp)` (AT&T: source then destination). Loads pull
from the same offsets into registers before arithmetic. Verbose asm comments
often name the C variable next to the access — trust but verify against your
declarations.

Frame allocation appears as `sub $N, %rsp` (or equivalent) after prologue setup.
`N` is not always exactly the sum of sizeof locals: alignment to 16 bytes for
ABI call requirements and compiler conservatism both inflate frames.

## Why addresses force stack homes

If you only compute with a local and never take its address, `-O2` may keep it in
a register forever. Taking `&x`, passing a pointer to another function, or
volatile-qualifying (later lesson) forces a memory identity. Printing through
`printf` also forces materialization because the value must exist where the call
ABI expects arguments.

So when the task says "touch each so they are not DCE'd," it means: give the
optimizer a reason the value or its address is observable. Dead-store and dead-
code elimination are not bugs; they are why empty-looking optimized asm appears.

## What `-O2` does to locals

Expect some combination of:

- Locals living only in general-purpose registers (`%eax`, `%rbx`, …).
- Unused locals disappearing completely.
- Reordered or coalesced slots if anything remains on the stack.
- Smaller or absent `sub $N,%rsp` when the frame is unnecessary.

The rejected wrong reading is "my variables are gone so the compiler is broken."
If the printed result is still correct, the values lived somewhere long enough —
just not where `-O0` left them.

## Worked example

**The situation.** In `work()` you declare `int a`, `long b`, `char c`, and
`int arr[4]`. You write each, then return an expression that mixes them (or print
them) so nothing is trivially dead. You emit asm at `-O0` and `-O2`.

**Step 1 — at `-O0`, list offsets.** Scan `work` for every `-N(%rbp)` store/load.
Map offsets to `a`, `b`, `c`, `arr[i]` using sizes and verbose comments. The
rejected wrong reading is: "lower offset always means earlier declaration." Slot
assignment is the compiler's choice; declaration order is a hint, not a
guarantee.

**Step 2 — measure the frame.** Find `sub $N,%rsp` (or `leave`-style teardown).
Compare `N` to the naive sum of sizes. The rejected wrong reading is: "`N` must
equal `sizeof(a)+…+sizeof(arr)`." Padding and alignment make exact equality
unlikely.

**Step 3 — repeat at `-O2`.** Note which offsets vanish and which values appear
only as immediate moves into registers. The rejected wrong reading is: "`-O2`
deleted `arr` so my program cannot use arrays." If you still return a sum of
elements, the array contents were handled; the *stack image* of the array may be
gone.

**Step 4 — optional debugger glance.** After `make`, a live debugger can print
addresses of locals at `-O0` and show they sit near `%rbp`. That confirms the
listing; it is not required to finish the mental model.

## Distinctions worth keeping straight

- **C name vs stack slot vs register** — three homes for one conceptual value.
- **Size vs alignment** — a `char` is one byte; its slot may still waste space.
- **Frame size vs sum of sizeof** — ABI alignment and compiler policy matter.
- **DCE vs bug** — unused locals vanishing at `-O2` is expected.

## Check yourself

1. In AT&T syntax, what does `movl %eax, -12(%rbp)` say about where a local lives?
2. Why might `sub $48,%rsp` appear when your locals only "need" 30 bytes?
3. What does taking `&local` discourage the optimizer from doing?
4. At `-O2`, if you cannot find `-N(%rbp)` for a local you definitely use, where
   might that value live instead?

## Key takeaways

- `-O0` locals are usually explicit stack slots off `%rbp`.
- Frame size includes alignment, not just raw sizeof sums.
- `-O2` may register-allocate or eliminate locals while preserving observable
  behavior.
- Read homes from `gcc -S`; do not assume declaration order equals offset order.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- `info as` — addressing mode spelling after you know the idea

Now open `TASK.md` and do the practice.
