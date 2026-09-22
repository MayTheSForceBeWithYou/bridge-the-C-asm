# 10 — Switch: jump table vs compares

A `switch` is not one machine instruction. GCC lowers it to either a *cascade of
compares* or a *jump table* (an array of addresses in `.rodata`, indexed by the
scrutinized value, followed by an indirect `jmp`). Dense contiguous case labels
favor tables; sparse labels often favor compares or hybrid schemes. By the end
of this lesson you should recognize both shapes in `.s` / `objdump` and say how
`-O0` versus `-O2` changes the choice.

## What this lesson asks of you

Implement `dense(int x)` with cases `0..7` contiguous, and `sparse(int x)` with
widely spaced cases (for example 0, 10, 100, 1000). Build at `-O0` and `-O2`.
Find evidence of tables versus compare chains. `TASK.md` is the lab.

## What `make` is naming for you

| You type | Open |
| -------- | ---- |
| `make asm` | `prog.s` — look for `.section .rodata` near `dense` and `jmp *` |
| `make O=2 asm` | copy aside — strategy may change |
| `make disasm` | `prog.lst` — search `<dense>:` / `<sparse>:` |

## Jump table shape — recognition rules

Typical ingredients in a PIC/`%rip`-relative GCC lowering:

```text
        cmpl    $7, -4(%rbp)      # bounds: is x > 7?
        ja      .Ldefault
        …
        leaq    .L4(%rip), %rdx   # base of table
        …                         # index into table
        jmp     *%rax             # indirect jump to case body

        .section .rodata
.L4:
        .long   .Lcase0-.L4
        .long   .Lcase1-.L4
        …
```

| Ingredient | What to look for | What it is *not* |
| ---------- | ---------------- | ---------------- |
| Bounds `cmpl $7` + `ja` | Reject out-of-range before indexing | Ordinary loop exit |
| `.section .rodata` + `.long .L…-.L4` | Relative offsets to case labels | String literals (also `.rodata`) |
| `jmp *%reg` | Indirect dispatch | `call *%reg` (function pointer — lesson 15) |

### Navigation move

1. Search `dense:` in `prog.s`.
2. Look for a bounds compare against the highest case (here `$7`) and an
   above/unsigned jump to default (`ja` is common — unsigned "above").
3. Search nearby for `.section .rodata` and a cluster of `.long` / `.quad`
   entries naming `.L*` labels.
4. Find `jmp *` — that is the dispatch.

**Rejected wrong reading:** "any `.rodata` mention means jump table" — string
literals also live there (lesson 12). Look for *code address* tables (label
differences) tied to the switch.

## Compare / branch shape

Sparse switches often become a tree or chain of `cmp` + conditional jumps —
essentially an optimized `if/else if` ladder:

```text
        cmpl    $1000, %edi
        je      .L_case1000
        cmpl    $100, %edi
        je      .L_case100
        …
        jmp     .L_default
```

You will not see a table of eight contiguous addresses if the labels are 0 and
1000 with nothing between; the compiler declines to waste a huge empty table
(unless optimizations invent a different strategy).

**Rejected wrong reading:** "sparse failed to optimize because I do not see a
table." Compares can be the *right* lowering.

## Optimization level

At `-O0`, expect the most literal structure — dense often gets a table early. At
`-O2`, GCC may switch strategies, reorder tests by balance, or convert between
table and compares. On some builds, a tiny dense switch might even become a
data table of *return values* instead of a jump table of code addresses. Do not
memorize one true lowering; learn the *signatures* so you can recognize whichever
you get.

If `-O2` removes the table you saw at `-O0`, your C semantics did not break —
only the dispatch strategy changed.

## Worked example

**The situation.** `dense` returns distinct small constants for cases 0–7 and a
default. `sparse` returns distinct constants for 0, 10, 100, 1000 and default.

**Step 1 — open `-O0` `dense`.** Search for `.rodata` address tables and `jmp *`.

**Step 2 — open `-O0` `sparse`.** Expect compare chains / trees.

**Step 3 — rebuild at `-O2`.** Diff strategies. Note anything that moved from
cascades to tables or vice versa (or to a value table).

**Step 4 — default path.** Find how out-of-range / unmatched values skip case
bodies. That bounds check is part of table safety.


## Relative jump tables on PIE

Modern GCC often stores *differences* (`.long .Lcase-.Ltable`) instead of
absolute addresses, then adds the table base at runtime. That keeps the table
PIC-friendly. When you dump `.rodata`, you may see small integers / offsets
rather than obvious code addresses — still a jump table if `dense` indexes it
and `jmp *` follows.

### What "value table" looks like (optional `-O2`)

Sometimes `-O2` replaces a jump table of *code* addresses with a table of
*return values* (load a constant from rodata by index). That is still
density-driven lowering — note it if you see a load from a table without
`jmp *`. Do not call it a failure of the lesson; call it a different signature.


## Bounds check is unsigned on purpose

A common dense pattern:

```text
        cmpl    $7, %edi
        ja      .Ldefault
```

`ja` (unsigned above) is deliberate: after treating negative `int` bit patterns
as huge unsigned values, they fall to default instead of indexing the table with
a nonsense index. When you see `ja` after a bounds `cmp` against the max case,
that is table safety — not proof the switch variable was declared `unsigned`.

### Sparse decision tree

Sparse lowering may compare against a middle value first (`cmp $100`), then
branch to lower/higher clusters. Sketch it as a tree, not a linear if-else, when
that is what the labels show.


## Checklist before you leave the listing

- [ ] Found bounds check for `dense` (or explained absence)
- [ ] Found either `jmp *` + address table **or** a value-table / compare lowering
- [ ] Described `sparse` with compare evidence
- [ ] Noted `-O0` vs `-O2` strategy change (or explicitly "same strategy")

## Distinctions worth keeping straight

- **Dense vs sparse case sets** — density drives strategy.
- **Jump table vs compare tree** — both are valid `switch` lowerings.
- **`.rodata` strings vs `.rodata` address tables** — same section family,
  different payload.
- **`jmp *` vs `call *`** — switch dispatch vs function pointer.
- **`-O0` vs `-O2` strategy** — may change without changing C results.

## Check yourself

1. What asm patterns suggest a jump-table `switch`?
2. Why might a sparse `switch` avoid a table?
3. Where does a bounds check fit in the table story?
4. If `-O2` removes the table you saw at `-O0`, did your C semantics break?
5. How do you distinguish a jump table in `.rodata` from a string literal?
6. What search helps find dispatch: `jmp *` or `.section .rodata` near `dense`?

## Key takeaways

- `switch` becomes compares, a jump table, or a hybrid — density matters.
- Tables live with addresses in rodata-like sections and dispatch via indirect
  jump; bounds checks protect them.
- Optimization can change the lowering; recognize signatures, not one template.
- Confirm with `gcc -S` and `objdump -d` across `-O` levels.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`, `-s`
- `man 5 elf` — section names

Now open `TASK.md` and do the practice.
