# 74 — Sections and symbols by hand
<!-- concepts: c.p5.elf.sections -->

Compilers sprinkle literals into `.rodata`, mutable globals into `.data`, and
zeroed storage into `.bss`. Exercise 11 read that layout out of `gcc` output;
here you author it with assembler directives and prove placement with `nm`,
`readelf`, `size`, and `objdump -s`.

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

## Directives you will actually use

- `.section .rodata` / `.data` / `.bss` (or shorthand `.data`, `.bss`).
- `.asciz "…"` / `.string` — NUL-terminated message bytes in `.rodata`.
- `.quad` / `.long` / `.byte` — initialized values in `.data`.
- `.space N` / `.zero N` — reserve `N` bytes (in `.bss`, zeros without storing
  them in the file).
- `.align` / `.balign` — pad so the next object meets an alignment (8 for
  `.quad`, 16 for some SIMD buffers).
- `.globl` / `.global`, `.type name, @object` / `@function`, `.size` — symbol
  metadata `nm` and debuggers consume.

Wrong section is a logic bug you can still link; wrong alignment is a runtime
fault waiting for a picky load or store.

## Reading the proof tools

- `nm -S --size-sort` — symbol type letters: often `T`/`t` text, `D`/`d` data,
  `B`/`b` bss, `R`/`r` rodata (exact letters vary by binding; compare yours).
- `readelf -S` — section names, sizes, flags (`A`, `W`, `X`).
- `size` — text / data / bss totals.
- `objdump -s -j .data -j .rodata` — hex dump of contents.

If you drop `.type` / `.size`, the program may still run; `readelf -s` and gdb
`print` become less informative — try the removal as a controlled experiment.

## Worked example

**The situation.** TODOs mark where the format string, `step`, `scratch`, and
`main` body belong.

**Step 1 — place objects.** Put the format in `.rodata`, give `step` a nonzero
initializer in `.data`, reserve `scratch` in `.bss`. The rejected wrong reading
is: "all globals go in `.data`." Zero-only buffers belong in `.bss` unless you
choose otherwise for teaching.

**Step 2 — write `main` to match expected stdout.** Drive `counter` / `step` /
totals per the stub's intended arithmetic until you see
`sections counter=7 total=12`. The rejected wrong reading is: "if it links, the
section choice was right." Prove with `nm` / `readelf`, not hope.

**Step 3 — migrate `scratch` to `.data`.** Assemble with `.space 64, 0` in
`.data`, re-run `size`, note the file-size delta, then keep the variant you
prefer.

## Distinctions worth keeping straight

- **File size vs runtime size** — `.bss` explains much of the gap.
- **`.rodata` write attempt** — should fault (often SIGSEGV); do not "fix" by
  putting constants in `.data` without noticing.
- **Alignment vs correctness of values** — aligned wrong can crash even when
  bytes look right in `objdump -s`.
- **`.type` / `.size` vs behavior** — metadata for tools; not the same as
  section choice.

## Check yourself

1. Which `nm` letter do you expect for `counter`, `step`, and `scratch`, and why
   might one differ?
2. Why does `.bss` cost address space but little file space?
3. What breaks if you omit `.align` before an 8-byte `.quad`?
4. Which object would a write through a `.rodata` pointer refuse, and with what
   signal?

## Key takeaways

- You choose sections with directives; tools prove where objects landed.
- `.rodata` / `.data` / `.bss` differ in mutability and on-disk footprint.
- Alignment directives are part of layout, not decoration.
- `nm`, `size`, `readelf -S`, and `objdump -s` are how you verify — not guess.

## Lookup (not the lesson)

- `info as` (nodes: Section, Data directives, Align)
- `man 1 nm`, `man 1 readelf`, `man 1 size`, `man 1 objdump`, `man 5 elf`

Now open `TASK.md` and do the practice.
