# 10 — Switch: jump table vs compares
<!-- concepts: c.p5.asm.jumptable -->

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

## Jump table shape

Typical ingredients:

- Bounds check (is `x` outside the table's index range?).
- Maybe a subtract if the lowest case is not zero.
- A load of a code address from a rodata table (often PIC/`%rip`-relative on
  modern GCC).
- `jmp *…` into the chosen case body (or into a block that then jumps).

Case bodies still look like ordinary basic blocks ending in jumps to a shared
`break`/exit label. The table is an implementation device for *dispatch*, not a
replacement for the case logic itself.

## Compare / branch shape

Sparse switches often become a tree or chain of `cmp` + conditional jumps —
essentially an optimized `if/else if` ladder. You will not see a table of eight
contiguous addresses if the labels are 0 and 1000 with nothing between; the
compiler declines to waste a huge empty table (unless optimizations invent a
different strategy).

## Optimization level

At `-O0`, expect the most literal structure. At `-O2`, GCC may switch strategies,
reorder tests by balance, or convert between table and compares. Do not memorize
one true lowering; learn the *signatures* so you can recognize whichever you get.

## Worked example

**The situation.** `dense` returns distinct small constants for cases 0–7 and a
default. `sparse` returns distinct constants for 0, 10, 100, 1000 and default.

**Step 1 — open `-O0` `dense`.** Search for `.rodata` references, address tables,
or `jmp *`. The rejected wrong reading is: "any `.rodata` mention means jump
table" — string literals also live there (lesson 12). Look for *code address*
tables or relocation patterns tied to the switch.

**Step 2 — open `-O0` `sparse`.** Expect compare chains. The rejected wrong
reading is: "sparse failed to optimize because I do not see a table." Compares
can be the *right* lowering.

**Step 3 — rebuild at `-O2`.** Diff strategies. Note anything that moved from
cascades to tables or vice versa.

**Step 4 — default path.** Find how out-of-range / unmatched values skip case
bodies. That bounds check is part of table safety.

## Distinctions worth keeping straight

- **Dense vs sparse case sets** — density drives strategy.
- **Jump table vs compare tree** — both are valid `switch` lowerings.
- **`.rodata` strings vs `.rodata` address tables** — same section family,
  different payload.
- **`-O0` vs `-O2` strategy** — may change without changing C results.

## Check yourself

1. What asm patterns suggest a jump-table `switch`?
2. Why might a sparse `switch` avoid a table?
3. Where does a bounds check fit in the table story?
4. If `-O2` removes the table you saw at `-O0`, did your C semantics break?

## Key takeaways

- `switch` becomes compares, a jump table, or a hybrid — density matters.
- Tables live with addresses in rodata-like sections and dispatch via indirect
  jump.
- Optimization can change the lowering; recognize signatures, not one template.
- Confirm with `gcc -S` and `objdump -d` across `-O` levels.

## Lookup (not the lesson)

- `man 1 gcc` — `-O`, `-S`
- `man 1 objdump` — `-d`, `-s`
- `man 5 elf` — section names

Now open `TASK.md` and do the practice.
