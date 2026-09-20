# 08 — Loop jump shapes

`for`, `while`, and `do`/`while` are C control-structure sugar over tests and
jumps. In assembly you see labels, conditional jumps (`jcc`), and sometimes an
unconditional `jmp` that closes a shape. By the end of this lesson you should
sketch a jump graph for each loop style at `-O0` and notice whether `-O2`
rewrites them into a common pattern (often test-at-bottom).

## What this lesson asks of you

Implement three functions that compute the same fold (sum `1..n` or sum an array)
using each loop keyword exactly once. Read `-O0` asm and optionally `-O2`.
`TASK.md` is the practice list.

## Reading a jump graph

Ignore prologue noise. Focus on:

- Where the **loop test** lives (before the body, after the body, or both).
- How you **enter** the first iteration (fall through vs jump-to-test).
- How you **exit** (conditional jump falling out of the loop).
- Whether an unconditional `jmp` hops back to a test or body label.

A `while` / `for` often tests first: if the condition fails, the body never runs.
A `do`/`while` runs the body once before testing. That semantic difference must
appear in the jump structure even when the arithmetic in the body looks identical.

## Counting jumps

At `-O0`, expect more jumps than you would like: compilers emit straightforward
shapes, not minimal ones. Count conditional versus unconditional jumps *per loop
construct*, not per whole file. When `-O2` enters, it may invert conditions,
rotate loops to test-at-bottom, or strength-reduce induction variables — your
graph may change even though results do not.

## Labels are just names

GCC invents labels like `.L2` / `.L3`. They are not C identifiers; they are
anchors for jumps. When you sketch a graph, rename them mentally to `test`,
`body`, and `done` if that helps — but keep the real names when citing evidence
in notes so your sketch still matches the file.

## Worked example

**The situation.** `sum_for`, `sum_while`, and `sum_do` all return `n*(n+1)/2`
for positive `n` (or handle `n==0` safely). You emit `-O0` asm.

**Step 1 — sketch `sum_while`.** Find the test label and the jump back. Mark
whether zero trips are possible without running the body. The rejected wrong
reading is: "there is a `jmp`, so this is a `goto` program and C loops are a
lie." Loops lower to jumps; that is expected.

**Step 2 — sketch `sum_do`.** Confirm the body label is reachable before the
test on first entry. The rejected wrong reading is: "`do`/`while` and `while`
must share identical asm because both use `while` in the name." Entry semantics
differ.

**Step 3 — sketch `sum_for`.** Relate init / test / step to labels. Note where
the step happens relative to the jump back.

**Step 4 — peek at `-O2`.** See if all three converged. If so, describe the
shared shape in one sentence (for example "test at bottom with initial
guard").

## Distinctions worth keeping straight

- **Source keyword vs jump shape** — keywords are input; jumps are output.
- **Test-at-top vs test-at-bottom** — zero-trip behavior differs.
- **Conditional vs unconditional jumps** — exit test versus always-back edges.
- **`-O0` pedagogy vs `-O2` rewrite** — both worth sketching.

## Check yourself

1. How can you tell from asm that a loop body might run zero times?
2. What extra guarantee does `do`/`while` give on the first iteration?
3. Why might `-O2` emit fewer labels than `-O0` for the same function?
4. If two loops compute the same sum, must their jump graphs match at `-O0`?

## Key takeaways

- Loops lower to labels + `jcc` / `jmp`.
- Test placement encodes zero-trip semantics.
- `-O2` may reshape different C loops into similar graphs.
- Sketch graphs from `.s`; do not argue only from keywords.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
