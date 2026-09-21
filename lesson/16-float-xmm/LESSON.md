# 16 — Float / double in xmm*

Integer arguments used general-purpose registers (lesson 04). Floating-point
arguments and results on SysV AMD64 use the SSE registers `%xmm0`, `%xmm1`, … for
scalar `float` / `double` in the ordinary cases you will meet here. Mixed calls
such as `double mix(double a, int n)` put the double in an XMM register and the
int in a GPR — separate channels. By the end of this lesson you should name which
XMM registers carry FP args/returns in your listings and recognize `addss` /
`addsd` / `mulss` / `mulsd`-class opcodes.

## What this lesson asks of you

Implement `addf`, `muld`, and a mixed `mix(double, int)`. Emit asm/disasm and
discover the FP ABI the same way you discovered the integer ABI: read the
compiler's output. `TASK.md` is the lab.

## Scalar SSE opcode names

The trailing letters hint at width:

- `ss` — scalar single (`float`),
- `sd` — scalar double (`double`).

So `addsd` adds two scalar doubles in XMM registers; `mulss` multiplies scalar
floats. You may also see moves like `movss` / `movsd` shuffling values between
XMM regs and memory.

## Separate register files

An integer `n` in `mix` will not ride in `%xmm0` just because there is also a
`double`. Watch the call site: `%xmm0` (typically) for the first FP arg, and a
GPR such as `%edi`/`%esi` for the first integer arg — both filled before `call`.
Returns of `float`/`double` similarly appear in `%xmm0` at `ret` time in the
common scalar cases.

Exact numbering for multiple FP args should come from *your* `addf` / `muld`
call sites — same discovery method as lesson 04.

## Why not `%st(0)` here

Older x87 stack registers still exist on the hardware, but the SysV AMD64
convention for ordinary scalar `float`/`double` arguments and returns is SSE
(`%xmm*`). Your GCC listings on this track should show XMM moves and `*ss`/`*sd`
ops for these exercises — if you see x87, treat it as a surprise worth
investigating, not the default expectation.

## Worked example

**The situation.** `addf(1.5f, 2.5f)`, `muld(2.0, 3.0)`, `mix(2.0, 3)` with printed
results.

**Step 1 — `addf` call site and body.** Note which XMM registers receive the
arguments and which holds the result at `ret`. The rejected wrong reading is:
"floats return in `%rax` like ints." Scalar FP returns use the XMM return
channel.

**Step 2 — `muld`.** Confirm double-width opcodes (`sd`) versus single (`ss`).
The rejected wrong reading is: "`double` uses two integer registers." It uses
scalar double SSE operations in ordinary SysV lowering.

**Step 3 — `mix`.** Identify XMM vs GPR assignment for `a` and `n`. Sketch the
two channels.

**Step 4 — contrast with lesson 04.** Write one line: "ints → GPRs; scalar FP →
XMMs." Keep it for the rest of the track.

## Distinctions worth keeping straight

- **GPR integer ABI vs XMM FP ABI** — parallel conventions.
- **`ss` vs `sd`** — float vs double scalar ops.
- **Mixed calls** — both channels filled independently.
- **Discovery from listings** — still no ABI PDF in the verified set.

## Check yourself

1. Which register class typically returns a `double`?
2. How do opcode suffixes `ss` and `sd` differ?
3. In `mix(double a, int n)`, where do you expect `a` and `n` at the call site?
4. Why is "everything goes in `%rdi`" the wrong mental default after this lesson?

## Key takeaways

- Scalar float/double args and results use XMM registers on SysV AMD64.
- Opcode suffixes encode single vs double scalar operations.
- Mixed functions use XMM *and* GPRs together.
- Discover numbering from `gcc -S`, as you did for integer args.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
