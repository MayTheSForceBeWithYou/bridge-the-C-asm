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

## What `make` is naming for you

| You type | File to open | Notes |
| -------- | ------------ | ----- |
| `make asm` | `prog.s` | Default `-O0` + `-fverbose-asm` |
| `make O=2 asm` | `prog.s` (overwrites) | Copy aside first if you need both |
| `make disasm` | `prog.lst` | Saved `objdump -d` of the linked binary |

## Stack slots at `-O0`

With a frame pointer in place (lesson 02), GCC typically assigns each live local
an offset below `%rbp`. An `int` often occupies 4 bytes; a `long` on LP64
occupies 8; a `char` occupies 1 but may still sit at an aligned offset with
padding around it. An array of four `int`s is sixteen bytes of consecutive slots.

### How to read one store line

A real `-O0` fragment for mixed locals looks like this (verbose comments included;
your exact offsets may differ — column meaning does not):

```text
        movl    $1, -4(%rbp)    #, a
        movq    $2, -16(%rbp)   #, b
        movb    $3, -17(%rbp)   #, c
        movl    $4, -48(%rbp)   #, arr[0]
        movl    $7, -36(%rbp)   #, arr[3]
```

Decode each operand:

| Piece | Example | What it is |
| ----- | ------- | ---------- |
| Mnemonic + width | `movl` / `movq` / `movb` | How many bytes are written (l=32, q=64, b=8). |
| Immediate source | `$1` | The constant being stored (AT&T: `$` means immediate). |
| Destination slot | `-4(%rbp)` | Memory at `%rbp - 4` — a local's home. |
| Verbose comment | `#, a` | GCC's hint naming the C entity (teaching aid; assembler ignores it). |

AT&T order is **source then destination**. So `movl $1, -4(%rbp)` means "write
the 32-bit value 1 into the stack slot at `%rbp-4`."

### How to navigate `work` without reading CRT

1. Open `prog.s`.
2. Search for `work:` (or whatever you named the function).
3. Skip prologue `pushq %rbp` / `movq %rsp, %rbp` and any `.cfi_*`.
4. Collect every `-N(%rbp)` that appears in stores/loads inside the body.
5. Use verbose comments (`#, a`, `#, arr[0]`) to map offsets back to C names —
   then verify sizes: `movl` to an `int` slot, `movq` to a `long`, `movb` to a
   `char`.

**Rejected wrong reading:** "lower offset always means earlier declaration." Slot
assignment is the compiler's choice; declaration order is a hint, not a
guarantee. In the sample above, `a` sits at `-4` while `b` sits at `-16` — do not
assume sequential packing from source order alone.

## Frame allocation and the `sub` you may or may not see

Frame allocation often appears as `subq $N, %rsp` after prologue setup. `N` is
not always exactly the sum of `sizeof` locals: alignment to 16 bytes for ABI call
requirements and compiler conservatism both inflate frames.

Sometimes at `-O0` GCC relies on the red zone (128 bytes below `%rsp` that signal
handlers leave alone on SysV) and **omits** an explicit `sub` for a leaf function.
Then locals still live at negative `%rbp` offsets, but you will not find a
`subq $N, %rsp` to quote as "the frame size." In that case, estimate the span
from the most negative offset you see (plus the size of that access), and say so
explicitly in your notes.

**Rejected wrong reading:** "`N` must equal `sizeof(a)+…+sizeof(arr)`." Padding
and alignment make exact equality unlikely — and a missing `sub` does not mean
locals are register-only at `-O0` when you can still see `-N(%rbp)` stores.

## Why addresses force stack homes

If you only compute with a local and never take its address, `-O2` may keep it in
a register forever. Taking `&x`, passing a pointer to another function, or
volatile-qualifying (lesson 18) forces a memory identity. Printing through
`printf` also forces materialization because the value must exist where the call
ABI expects arguments.

So when the task says "touch each so they are not DCE'd," it means: give the
optimizer a reason the value or its address is observable. Dead-store and
dead-code elimination are not bugs; they are why empty-looking optimized asm
appears.

## What `-O2` does to locals

Expect some combination of:

- Locals living only in general-purpose registers (`%eax`, `%rbx`, …).
- Unused locals disappearing completely.
- Reordered or coalesced slots if anything remains on the stack.
- Smaller or absent `sub $N,%rsp` when the frame is unnecessary.

### How to compare without getting lost

1. Keep `prog-O0.s` and `prog-O2.s` as separate files.
2. Search `work:` in each.
3. In the `-O2` body, ask: do I still see `-N(%rbp)`? Do I see immediate moves
   straight into registers used by the return expression?
4. If the printed/returned result is still correct, the values lived somewhere —
   just not where `-O0` left them.

**Rejected wrong reading:** "my variables are gone so the compiler is broken."
If the printed result is still correct, the values lived somewhere long enough.

**Rejected wrong reading:** "`-O2` deleted `arr` so my program cannot use arrays."
If you still return a sum of elements, the array contents were handled; the
*stack image* of the array may be gone.

## Worked example

**The situation.** In `work()` you declare `int a`, `long b`, `char c`, and
`int arr[4]`. You write each, then return an expression that mixes them (or print
them) so nothing is trivially dead. You emit asm at `-O0` and `-O2`.

**Step 1 — at `-O0`, list offsets.** Scan `work` for every `-N(%rbp)` store/load.
Map offsets to `a`, `b`, `c`, `arr[i]` using widths and verbose comments. Example
reading from the sample: `a` at `-4` (`movl`), `b` at `-16` (`movq`), `c` at
`-17` (`movb`), `arr[0]` at `-48`, `arr[3]` at `-36` (array slots 4 bytes apart).

**Step 2 — measure the frame.** Find `subq $N,%rsp` if present, or estimate from
the deepest slot. Compare to the naive sum of sizes (~4+8+1+16). Expect a gap.

**Step 3 — repeat at `-O2`.** Note which offsets vanish and which values appear
only as immediate moves into registers.

**Step 4 — optional debugger glance.** After `make`, a live debugger can print
`&a` at `-O0` and show it sits near `%rbp`. That confirms the listing; it is not
required to finish the mental model.


## Reading the same slots in `objdump -d`

After `make disasm`, search `<work>:`. A store still has lesson-01 columns:

```text
    1150:       c7 45 fc 01 00 00 00    movl   $0x1,-0x4(%rbp)
```

| Column | Example | Meaning |
| ------ | ------- | ------- |
| Address | `1150:` | where this store lives |
| Bytes | `c7 45 fc 01 00 00 00` | encoding of `movl $1, -4(%rbp)` |
| Asm | `movl $0x1,-0x4(%rbp)` | same slot story as in `.s` |

Negative displacements may appear as `-0x4(%rbp)` in objdump and `-4(%rbp)` in
`.s` — same offset. Do not treat the `0x` form as a different kind of number.

### Array layout on the stack

For `int arr[4]` at `-O0`, expect four `movl` stores at offsets 4 bytes apart.
If `arr[0]` is at `-48(%rbp)`, then `arr[1]` is often `-44`, `arr[2]` `-40`,
`arr[3]` `-36` — ascending addresses as the index increases (stack grows down,
but array indices still go toward higher addresses).

## Distinctions worth keeping straight

- **C name vs stack slot vs register** — three homes for one conceptual value.
- **Size vs alignment** — a `char` is one byte; its slot may still waste space.
- **Frame size vs sum of sizeof** — ABI alignment and compiler policy matter.
- **Explicit `sub` vs red-zone leaf** — missing `sub` ≠ "no stack locals."
- **DCE vs bug** — unused locals vanishing at `-O2` is expected.
- **Mnemonic width vs C type** — `movl` is 32-bit; match it to `int`, not
  blindly to "the next local."

## Check yourself

Close this file and answer from memory, then verify against your artifacts:

1. In AT&T syntax, what does `movl %eax, -12(%rbp)` say about where a local lives?
2. Why might `sub $48,%rsp` appear when your locals only "need" 30 bytes?
3. What does taking `&local` discourage the optimizer from doing?
4. At `-O2`, if you cannot find `-N(%rbp)` for a local you definitely use, where
   might that value live instead?
5. How do you tell `arr[0]` from `arr[1]` in a listing when both use `movl`?
6. If there is no `subq` in a leaf `work` at `-O0`, does that mean locals are not
   on the stack?

## Key takeaways

- `-O0` locals are usually explicit stack slots off `%rbp`.
- Decode `-N(%rbp)` together with the mnemonic width and verbose comment.
- Frame size includes alignment, not just raw sizeof sums — and may lack an
  explicit `sub` on leaf functions.
- `-O2` may register-allocate or eliminate locals while preserving observable
  behavior.
- Read homes from `gcc -S`; do not assume declaration order equals offset order.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`, `-fverbose-asm`
- `man 1 objdump` — `-d`
- `info as` — addressing mode spelling after you know the idea

Now open `TASK.md` and do the practice.
