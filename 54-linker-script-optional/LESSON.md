# 54 — Optional: a custom linker script section

Linker scripts tell `ld` how to map input sections into the output ELF — addresses,
order, and which segments exist. This optional stretch has you keep a custom
`.mysec` alive in the output and verify it with `readelf -S`. Skip if short on time;
Tracks F and G do not depend on it.

## What this lesson asks of you

Define a `.mysec` (assembler `.section` or C `section` attribute) with a global symbol.
Write a minimal `link.ld` that preserves normal linking needs while ensuring `.mysec`
appears. Link with `ld -T link.ld`, confirm via `readelf -S`, and keep the program
runnable (exit 0).

## What a linker script is doing

Default scripts (inside binutils) already place `.text`, `.data`, `.bss`, and friends.
Your script can `INCLUDE` defaults or draft a small SECTIONS block that emits `.mysec`
with suitable flags (`"aw"` for allocatable writable data is a common lab choice).

You are not expected to receive a handed-out perfect script — drafting from `info ld`
is the point. Wrong scripts fail the link or drop the section; iterate.

## Worked example

**The situation.** `readelf -S ./prog` lists `.mysec` with expected flags; `./prog`
still exits 0.

**Step 1 — create the section in the object.** Assembler: `.section .mysec, "aw"` plus
a `.globl` symbol. The rejected wrong reading is: "if I declare a symbol, ld will invent
a section name automatically matching my C variable." Section names are explicit; the
script matches on those names.

**Step 2 — script and link.** `ld -T link.ld -o prog start.o`. The rejected wrong
reading is: "linker scripts only matter for kernels and embedded." They also matter for
custom layouts, embedded ROMs, and unusual section tricks on desktop Linux.

**Step 3 — verify.** `readelf -S` shows `.mysec`. Program still runs.

## Distinctions worth keeping straight

- **Input section vs output section** — objects provide inputs; the script maps them to
  outputs.
- **Flags `"aw"` vs executable text** — permissions must match usage.
- **Optional stretch vs required Track E core** — skip cleanly if needed; do not fake
  completion.
- **`-T script` vs default** — replacing the script means you own placement.

## Check yourself

1. How do you ask `ld` to use your script?
2. Which tool lists section names and flags in the final ELF?
3. Why might a custom section disappear despite existing in `start.o`?
4. When is skipping this exercise acceptable?

## Key takeaways

- Linker scripts control output section layout.
- Custom sections must be both emitted by objects and accepted by the script.
- `readelf -S` verifies presence and flags.
- This stretch is optional; judgment about time is part of the track.

## Lookup (not the lesson)

- `info ld`, `man 1 ld`, `man 1 readelf`, `man 5 elf`, `man 1 as`

Now open `TASK.md` and do the practice.
