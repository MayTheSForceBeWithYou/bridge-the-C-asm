# 74 — Sections and symbols by hand

Compilers sprinkle literals into `.rodata`, mutable globals into `.data`, and
zeroed storage into `.bss`. Exercise 11 read that layout out of `gcc` output;
here you author it with assembler directives and prove placement with `nm`,
`readelf`, `size`, and `objdump -s`. By the end you should decode `nm` type
letters, explain why `.bss` is cheap on disk, and treat `.align` as load-bearing
layout rather than decoration.

## What this lesson asks of you

Fill `sections_asm.s`: a format string in `.rodata`, `step` in `.data`,
`scratch` in `.bss`, and a `main` that uses them so stdout becomes
`sections counter=7 total=12`. Then move `scratch` into `.data` once and watch
`size` change.

## Three data homes

| Section | Typical contents | On disk | At runtime |
| ------- | ---------------- | ------- | ---------- |
| `.rodata` | string literals, `const` | bytes present | mapped read-only |
| `.data` | mutable globals with nonzero init | bytes present | read-write |
| `.bss` | zero-initialized globals | little/no payload | zero-filled by loader |

`.bss` costs address space but not file bytes (beyond a size in the ELF). That
is why `size` shows a bss column separate from data, and why moving a large zero
buffer from `.bss` into `.data` with `.space 64, 0` can grow the file.

**Rejected wrong reading:** "All globals go in `.data`." Zero-only buffers
belong in `.bss` unless you deliberately put them in `.data` for a teaching
experiment.

## Directives you will actually use — decode as fields

| Directive | What it does | Recognition tip |
| --------- | ------------ | ---------------- |
| `.section .rodata` / `.data` / `.bss` | switch output section | later bytes land here |
| `.asciz "…"` / `.string` | NUL-terminated message bytes | belongs in `.rodata` |
| `.quad` / `.long` / `.byte` | initialized values | `.data` for mutable |
| `.space N` / `.zero N` | reserve `N` bytes | in `.bss` = no file payload |
| `.align` / `.balign` | pad to alignment | 8 for `.quad`, 16 for some buffers |
| `.globl` / `.global` | export symbol | linker / `nm` visibility |
| `.type name, @object` / `@function` | symbol kind metadata | helps debuggers |
| `.size` | symbol size metadata | `nm -S` shows sizes better |

Wrong section is a logic bug you can still link; wrong alignment is a runtime
fault waiting for a picky load or store.

**Rejected wrong reading:** "If it links, the section choice was right." Prove
with `nm` / `readelf`, not hope.

## Reading the proof tools — column / field decoding

### `nm -S --size-sort`

A typical line looks like:

```text
0000000000004010 0000000000000008 D step
```

| Field | Example | Meaning |
| ----- | ------- | ------- |
| Address | `0000000000004010` | where the symbol lives (link view) |
| Size | `0000000000000008` | from `-S`; eight bytes here |
| Type letter | `D` | usually data (`.data`); see table below |
| Name | `step` | your symbol |

Common type letters (binding/case varies — compare yours):

| Letter | Often means |
| ------ | ----------- |
| `T` / `t` | text / code |
| `D` / `d` | `.data` |
| `B` / `b` | `.bss` |
| `R` / `r` | `.rodata` |

**Navigation:** `nm -S --size-sort sections_asm | grep -Ei 'counter|step|scratch'`
so you are not reading the whole table.

### `readelf -S`

Search for section names `.text`, `.rodata`, `.data`, `.bss`. Note size and
flags (`A` alloc, `W` write, `X` execute). `.rodata` should not be `W`;
`.data` should be.

### `size`

Three totals — text / data / bss. Moving `scratch` from `.bss` into `.data`
should grow the data column (and often the on-disk file) while shrinking bss.

### `objdump -s -j .data -j .rodata`

Hex dump of contents. **Recognition:** your format string's ASCII should appear
in the `.rodata` dump; `step`'s initializer bytes in `.data`. If the string is
missing from `.rodata`, you put it in the wrong section.

## What `make` maps to

| You type | Result |
| -------- | ------ |
| `make && make run` | expected stdout line |
| `nm` / `readelf -S` / `size` / `objdump -s` | proof of placement |
| Move `scratch` to `.data`, rebuild `size` | observe delta |

## Worked example

**The situation.** TODOs mark where the format string, `step`, `scratch`, and
`main` body belong.

**Step 1 — place objects.** Put the format in `.rodata`, give `step` a nonzero
initializer in `.data`, reserve `scratch` in `.bss`. Use `.align` before
multi-byte objects if the stub or ABI width calls for it.

**Step 2 — write `main` to match expected stdout.** Drive `counter` / `step` /
totals per the stub's intended arithmetic until you see
`sections counter=7 total=12`. Do not invent a different message and call it
done.

**Step 3 — prove with tools.** Run the `nm` / `readelf` / `size` / `objdump -s`
commands from the task. Match type letters to the table above. The rejected
wrong reading is stopping at "it printed the right line" without checking
sections.

**Step 4 — migrate `scratch` to `.data`.** Assemble with `.space 64, 0` in
`.data`, re-run `size`, note the file-size / data-column delta, then keep the
variant you prefer for the checked-in solution path.

**Step 5 — optional metadata experiment.** Temporarily drop `.type` / `.size`;
confirm the program may still run while `readelf -s` / gdb become less
informative. Restore them — they are for tools, not for CPU fetch.

## Distinctions worth keeping straight

- **File size vs runtime size** — `.bss` explains much of the gap.
- **`.rodata` write attempt** — should fault (often SIGSEGV); do not "fix" by
  putting constants in `.data` without noticing.
- **Alignment vs correctness of values** — aligned wrong can crash even when
  bytes look right in `objdump -s`.
- **`.type` / `.size` vs behavior** — metadata for tools; not the same as
  section choice.
- **`nm` address vs `objdump -d` instruction address** — both hex; different
  objects (data symbol vs instruction).

## Check yourself

1. Which `nm` letter do you expect for `counter`, `step`, and `scratch`, and why
   might one differ?
2. Why does `.bss` cost address space but little file space?
3. What breaks if you omit `.align` before an 8-byte `.quad`?
4. Which object would a write through a `.rodata` pointer refuse, and with what
   signal?
5. On an `nm -S` line, which field is size versus type letter?
6. Which `objdump` flags dump `.rodata` contents for you to search for your
   format string?

## Key takeaways

- You choose sections with directives; tools prove where objects landed.
- `.rodata` / `.data` / `.bss` differ in mutability and on-disk footprint.
- Alignment directives are part of layout, not decoration.
- Decode `nm` / `size` / `readelf -S` / `objdump -s` field by field — not as a
  blob of hex.
- Linking success is not section proof.


## Expected stdout as a layout smoke test

`sections counter=7 total=12` is not arbitrary — it proves `main` actually read
and wrote the objects you placed. If sections are wrong but values happen to
match from registers alone, you cheated the pedagogy. Force yourself to load
from `step` / `scratch` / the format string so `nm` and stdout agree.

If stdout is right but `scratch` shows up as `D` instead of `B`, you put it in
`.data` — fine for the migration experiment, wrong for the default TODO unless
you already moved it on purpose.

## Lookup (not the lesson)

- `info as` (nodes: Section, Data directives, Align)
- `man 1 nm`, `man 1 readelf`, `man 1 size`, `man 1 objdump`, `man 5 elf`

Now open `TASK.md` and do the practice.
