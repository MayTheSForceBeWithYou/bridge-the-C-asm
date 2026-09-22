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

## Mapping commands to checks

| You type | Check afterward |
| -------- | --------------- |
| assemble object with `.mysec` | `readelf -S start.o` lists `.mysec` |
| `ld -T link.ld -o prog …` | link succeeds |
| `readelf -S ./prog` | `.mysec` survives with expected flags |
| `./prog` | still exits 0 |

## Input section vs output section

Objects provide *input* sections; the linker script maps them to *output* sections.
**Rejected wrong reading:** declaring a symbol invents a matching section name
automatically — section names are explicit.

## Why a custom section disappears

If the script never mentions `.mysec` (and does not use a catch-all that keeps it), `ld`
may drop it. **Navigation:** compare `readelf -S` on the `.o` vs the final ELF.

## Optional stretch judgment

Tracks F/G do not depend on this. Skip cleanly if short on time — do not fake completion.

## Flags like `"aw"`

Permissions must match usage (allocatable writable data vs executable text). Mis-flagging
is a common silent layout bug.

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

## Minimal script sketch (illustrative — iterate)

A teaching script often `SECTIONS` your `.mysec` with suitable flags and still places
`.text`/`.data`/`.bss`. Wrong scripts fail the link or drop the section — that iteration
*is* the lesson.

**Verification navigation:** `readelf -S ./prog` then search `.mysec`. Confirm flags match
intent (`A` allocatable, `W` writable, etc.).

## When skipping is correct

Optional stretch — Track F/G do not depend on it. Skip cleanly rather than invent a fake
section.

## Distinctions worth keeping straight

- **Input section vs output section** — objects provide inputs; the script maps them to
  outputs.
- **Flags `"aw"` vs executable text** — permissions must match usage.
- **Optional stretch vs required Track E core** — skip cleanly if needed; do not fake
  completion.
- **`-T script` vs default** — replacing the script means you own placement.

## Deeper worked navigation (linker script optional)

- Objects emit sections; scripts map them.
- readelf -S verifies survival.
- Optional — skip cleanly if needed.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| readelf -S .o | .mysec present |
| ld -T | link |
| readelf -S prog | .mysec survived |

### Ordered navigation moves

1. Create section.
2. Draft script.
3. Link+verify.
4. Keep runnable.

### Rejected wrong readings (keep beside the artifact)

- symbol invents section name.
- scripts only for kernels.
- fake completion if skipping.

### Tool-line decoding reminders

- input vs output section
- section flags
- -T replaces default layout

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (54)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - symbol invents section
- never verified readelf -S
- faked optional completion

    For each tick: write the *recognition* fix (which register/column/anchor) in one line.
    That diary is how this track sticks.

    ## Makefile → command → file (recap)

    | You type | Produces / runs | Open next |
    | -------- | --------------- | --------- |
    | `make` / `make bin` | exercise binary | run it; note exit status |
    | `make clean` | removes objects | before changing `O=` / flags |
    | `make asm` / `make disasm` (if any) | listing view | search the label you care about |
    | tools in Lookup | field dumps | decode columns, do not skim blobs |

    Remember: a disasm target usually *views* bytes already linked — it is not a new
    mysterious compile stage (lesson 01's `.lst` rule).

## Check yourself

1. How do you ask `ld` to use your script?
2. Which tool lists section names and flags in the final ELF?
3. Why might a custom section disappear despite existing in `start.o`?
4. When is skipping this exercise acceptable?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

Anchor drill: write the exact search string you used in the primary listing, then
quote one real line from your machine with every field labeled. If you cannot label
a field, that field is still unlearned — return to the decoding table above.

## Key takeaways

- Linker scripts control output section layout.
- Custom sections must be both emitted by objects and accepted by the script.
- `readelf -S` verifies presence and flags.
- This stretch is optional; judgment about time is part of the track.

## Field-decoding recap for exercise 54

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `info ld`, `man 1 ld`, `man 1 readelf`, `man 5 elf`, `man 1 as`

Now open `TASK.md` and do the practice.
