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

## What `make` is naming for you

| You type | Open |
| -------- | ---- |
| `make asm` | `prog.s` — labels `.L2`, `.L3`, … inside each function |
| `make O=2 asm` | copy aside first — shapes may collapse |
| `make disasm` | `prog.lst` — search `<sum_while>:` etc. |

## Reading a jump graph

Ignore prologue noise. Focus on:

- Where the **loop test** lives (before the body, after the body, or both).
- How you **enter** the first iteration (fall through vs jump-to-test).
- How you **exit** (conditional jump falling out of the loop).
- Whether an unconditional `jmp` hops back to a test or body label.

### How to extract a graph from `.s`

1. Search `sum_while:` (or your name).
2. List every label inside that function (`.L2:`, `.L3:`, …).
3. List every `j*` / `jmp` and its target label.
4. Mentally rename labels to `test`, `body`, `done` *in your sketch*, but keep
   the real `.L*` names when citing evidence so the sketch still matches the file.

A `while` / `for` often tests first: if the condition fails, the body never runs.
A `do`/`while` runs the body once before testing. That semantic difference must
appear in the jump structure even when the arithmetic in the body looks identical.

### Recognizing test-at-top vs test-at-bottom

| Shape | Typical evidence | Zero-trip (`n==0`) |
| ----- | ---------------- | ------------------ |
| Test-at-top | Early `cmp`/`test` + `jcc` to `done` before body label | Body skipped |
| Test-at-bottom | Body first; `cmp` at end + `jcc` back to body; maybe a guard jump for empty | Needs initial guard or separate path |
| `do`/`while` | First instruction of payload reachable without passing a failing test | Body runs at least once |

**Rejected wrong reading:** "there is a `jmp`, so this is a `goto` program and C
loops are a lie." Loops lower to jumps; that is expected.

**Rejected wrong reading:** "`do`/`while` and `while` must share identical asm
because both use `while` in the name." Entry semantics differ.

## Counting jumps

At `-O0`, expect more jumps than you would like: compilers emit straightforward
shapes, not minimal ones. Count conditional versus unconditional jumps *per loop
construct*, not per whole file. When `-O2` enters, it may invert conditions,
rotate loops to test-at-bottom, or strength-reduce induction variables — your
graph may change even though results do not.

Example annotation style for notes:

```text
sum_while:
  .L_test:  cmp … ; jle .L_done
  .L_body:  … ; jmp .L_test
  .L_done:
  # conditional: 1   unconditional: 1
```

## Labels are just names

GCC invents labels like `.L2` / `.L3`. They are not C identifiers; they are
anchors for jumps. In `objdump` listings you may see addresses instead of those
names on jump targets — still the same graph.

## Worked example

**The situation.** `sum_for`, `sum_while`, and `sum_do` all return `n*(n+1)/2`
for positive `n` (or handle `n==0` safely). You emit `-O0` asm.

**Step 1 — sketch `sum_while`.** Find the test label and the jump back. Mark
whether zero trips are possible without running the body.

**Step 2 — sketch `sum_do`.** Confirm the body label is reachable before the
test on first entry.

**Step 3 — sketch `sum_for`.** Relate init / test / step to labels. Note where
the step happens relative to the jump back.

**Step 4 — peek at `-O2`.** See if all three converged. If so, describe the
shared shape in one sentence (for example "test at bottom with initial guard").


## Sketch template (copy into your notes)

```text
Function: sum_while
Labels:
  .L2:  role = ________
  .L3:  role = ________
Edges:
  jcc  .L?   condition = ________   (exit / continue?)
  jmp  .L?   (back-edge?)
Zero-trip possible without body?  yes / no
Conditional jump count: _
Unconditional jump count: _
```

Fill one template per function at `-O0`, then optionally a second set at `-O2`.
The templates make "converged under optimization" a factual claim instead of a
vibe.

### Objdump jumps

In `prog.lst`, conditional jumps show targets as addresses or symbolized labels.
Still graph them: from instruction address A, edge to address B. The `.s` label
names are easier for sketching; the `.lst` confirms the same edges survived
linking.


## Worked mini-trace for `while`

Suppose `sum_while` at `-O0` contains (names illustrative):

```text
        jmp     .L_test
.L_body:
        … add …
.L_test:
        cmpl    …
        jle     .L_body
```

Or the more common "test first by falling into a compare":

```text
.L_test:
        cmpl    …
        jg      .L_done
        … body …
        jmp     .L_test
.L_done:
```

Either way, mark where the body sits relative to the first failing test. For
`do`/`while`, the body label should be the entry before any exit test — that is
the structural fingerprint you came for.

### Counting discipline

Count only jumps whose target is inside the same function's loop construct. A
`jmp` to `.LFE` or out to epilogue may be a function exit, not a loop back-edge.
Annotate each jump as `back-edge`, `exit`, or `enter-test` in your sketch.

## Distinctions worth keeping straight

- **Source keyword vs jump shape** — keywords are input; jumps are output.
- **Test-at-top vs test-at-bottom** — zero-trip behavior differs.
- **Conditional vs unconditional jumps** — exit test versus always-back edges.
- **`.L*` names vs your mental `test`/`body`/`done`** — cite real names.
- **`-O0` pedagogy vs `-O2` rewrite** — both worth sketching.

## Check yourself

1. How can you tell from asm that a loop body might run zero times?
2. What extra guarantee does `do`/`while` give on the first iteration?
3. Why might `-O2` emit fewer labels than `-O0` for the same function?
4. If two loops compute the same sum, must their jump graphs match at `-O0`?
5. What is the difference between `jle .Ldone` and `jmp .Ltest` in a sketch?
6. How do you avoid counting jumps that belong to `main` or CRT?

## Key takeaways

- Loops lower to labels + `jcc` / `jmp`.
- Test placement encodes zero-trip semantics — read the graph, not the keyword.
- `-O2` may reshape different C loops into similar graphs.
- Sketch graphs from `.s`; do not argue only from keywords.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`

Now open `TASK.md` and do the practice.
