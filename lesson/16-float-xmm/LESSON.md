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

## What `make` is naming for you

| You type | Open | Search |
| -------- | ---- | ------ |
| `make asm` | `prog.s` | `addf:`, `muld:`, `mix:`, `xmm`, `addss`/`mulsd` |
| `make disasm` | `prog.lst` | `<addf>:`, watch `%xmm0` / `%xmm1` |

## Scalar SSE opcode names

The trailing letters hint at width:

| Suffix | Meaning | Example |
| ------ | ------- | ------- |
| `ss` | scalar single (`float`) | `addss`, `mulss`, `movss` |
| `sd` | scalar double (`double`) | `addsd`, `mulsd`, `movsd` |

So `addsd` adds two scalar doubles in XMM registers; `mulss` multiplies scalar
floats. Moves like `movss` / `movsd` shuffle values between XMM regs and memory.

## Separate register files — decode a call site

An integer `n` in `mix` will not ride in `%xmm0` just because there is also a
`double`. Watch the call site: `%xmm0` (typically) for the first FP arg, and a
GPR such as `%edi` for the first integer arg — both filled before `call`.

Real `-O0` callee shapes:

```text
addf:   # float a, float b
        movss   %xmm0, -4(%rbp)    # spill a
        movss   %xmm1, -8(%rbp)    # spill b
        movss   -4(%rbp), %xmm0
        addss   -8(%rbp), %xmm0    # result in %xmm0
        ret

muld:   # double a, double b
        movsd   %xmm0, -8(%rbp)
        movsd   %xmm1, -16(%rbp)
        movsd   -8(%rbp), %xmm0
        mulsd   -16(%rbp), %xmm0
        ret

mix:    # double a, int n
        movsd   %xmm0, -8(%rbp)    # a
        movl    %edi, -12(%rbp)    # n in GPR channel
        …
        cvtsi2sdl -12(%rbp), %xmm0
        mulsd   -8(%rbp), %xmm0
        ret
```

| Channel | First arg home (typical) | Return home (scalar FP) |
| ------- | ------------------------ | ----------------------- |
| `float`/`double` args | `%xmm0`, `%xmm1`, … | `%xmm0` |
| `int` in a mixed call | `%edi` / `%rdi` class | n/a (unless returning int) |

Exact numbering for multiple FP args should come from *your* `addf` / `muld`
call sites in `main` — same discovery method as lesson 04 (distinctive constants
help less for floats, but distinctive *literals* and verbose comments still do).

### Navigation

1. Search `addf:` — note spills from `%xmm0`/`%xmm1` and the `addss` into `%xmm0`.
2. Search `muld:` — confirm `sd` opcodes and `%xmm0` result.
3. Search `mix:` — confirm `%xmm0` + `%edi` both appear on entry.
4. Search `main:` call sites — see which registers are loaded before each `call`.

**Rejected wrong reading:** "floats return in `%rax` like ints." Scalar FP
returns use the XMM return channel.

**Rejected wrong reading:** "`double` uses two integer registers." It uses
scalar double SSE operations in ordinary SysV lowering.

## Why not `%st(0)` here

Older x87 stack registers still exist on the hardware, but the SysV AMD64
convention for ordinary scalar `float`/`double` arguments and returns is SSE
(`%xmm*`). Your GCC listings on this track should show XMM moves and `*ss`/`*sd`
ops for these exercises — if you see x87 (`fld`, `fadd`, `%st`), treat it as a
surprise worth investigating, not the default expectation.

## Worked example

**The situation.** `addf(1.5f, 2.5f)`, `muld(2.0, 3.0)`, `mix(2.0, 3)` with printed
results.

**Step 1 — `addf` call site and body.** Note which XMM registers receive the
arguments and which holds the result at `ret`.

**Step 2 — `muld`.** Confirm double-width opcodes (`sd`) versus single (`ss`).

**Step 3 — `mix`.** Identify XMM vs GPR assignment for `a` and `n`. Sketch the
two channels in one sentence.

**Step 4 — contrast with lesson 04.** Write one line: "ints → GPRs; scalar FP →
XMMs." Keep it for the rest of the track.


## Call-site checklist for `mix`

Before `call mix`, your `main` should show *both*:

1. Something loading a double into `%xmm0` (literal pool load / `movsd`), and
2. Something loading the int into `%edi` / `%esi` / … 

If you only find one channel, you are looking at the wrong call or the wrong
optimization level's inlining. Search `call mix` and read upward.

### Return consume

After `call addf`, the float result is in `%xmm0` — callers may `movss` it to
the stack for `printf` or convert for varargs. Do not hunt for the float in
`%eax` unless you see an explicit move there.


## Spill does not change the ABI home

At `-O0`, `movss %xmm0, -4(%rbp)` looks like "the float lives on the stack."
True as a spill; false as ABI folklore. The argument still *arrived* in `%xmm0`.
Same lesson as integer args spilling to `-8(%rbp)` in lesson 04.


## Discovery table (fill from *your* `.s`)

```text
addf args:  xmm____  xmm____     return: xmm____
muld args:  xmm____  xmm____     return: xmm____
mix  args:  xmm____  (GPR ____)  return: xmm____
sample opcode from body: ________
```


## Rejected wrong reading (mixed calls)

"**In `mix(double a, int n)`, both arguments pack into `%xmm0` because the first
is floating.**" Integer and FP args use separate register files. Expect `%xmm0`
*and* a GPR.

## Distinctions worth keeping straight

- **GPR integer ABI vs XMM FP ABI** — parallel conventions.
- **`ss` vs `sd`** — float vs double scalar ops.
- **Mixed calls** — both channels filled independently.
- **Spill at `-O0` vs ABI home** — spilling `%xmm0` to the stack does not change
  where the arg arrived.
- **Discovery from listings** — still no ABI PDF in the verified set.

## Check yourself

1. Which register class typically returns a `double`?
2. How do opcode suffixes `ss` and `sd` differ?
3. In `mix(double a, int n)`, where do you expect `a` and `n` at the call site?
4. Why is "everything goes in `%rdi`" the wrong mental default after this lesson?
5. What does `movss %xmm0, -4(%rbp)` in `addf` tell you — ABI violation or `-O0`
   spill?
6. Which search tokens find FP ops quickly (`xmm`, `addss`, `mulsd`)?

## Key takeaways

- Scalar float/double args and results use XMM registers on SysV AMD64.
- Opcode suffixes encode single vs double scalar operations.
- Mixed functions use XMM *and* GPRs together.
- Discover numbering from `gcc -S`, as you did for integer args.

## Lookup (not the lesson)

- `man 1 gcc` — `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
