# 52 — Stripping symbols from a running binary

`strip` removes symbol table and debug-related sections that humans and debuggers love
— but the machine code and the information the loader needs can remain. A stripped copy
often still runs, shrinks on disk, and makes `nm` far less helpful. This lesson separates
"needed to execute" from "nice for debugging," contrasting with exercise 38's
gdb-by-address recovery theme.

## What this lesson asks of you

Build a working hello/`prog`, copy it, `strip` the copy, show it still runs, capture
`nm`/`size` before and after, and note what disappeared versus what execution still
required.

## What strip removes

Typically: static symbol table entries, debug info (if present), some non-loaded
metadata. What stays: `.text`, essential loadable segments, dynamic linking info needed
at runtime for dynamic binaries (dynamic symbols may remain in some forms — observe
yours).

`nm` on a fully stripped binary often fails or prints almost nothing useful. `size`
usually drops. `./prog.stripped` still prints hello.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | unstripped `./prog` |
| copy then `strip` | `./prog.stripped` still runs |
| `nm` before/after | rich → empty/error |
| `size` before/after | usually shrinks |

## What strip removes vs what execution needs

Typically removed: static symbol table, debug info. Typically kept: `.text`, loadable
segments, dynamic info needed at runtime for dynamic binaries.

**Rejected wrong reading:** "symbols are how the CPU finds `main` at runtime." Calls use
addresses already fixed in machine code / GOT; symbol *names* are metadata.

**Rejected wrong reading:** "if `nm` fails, the binary cannot run." Running tests
execution; `nm` tests symbol presence.

## Always copy before stripping

Keep an unstripped build for debugging (exercise 38's theme). Strip the copy.

## Strip vs visibility (50)

Visibility = compile/link export policy. Strip = post-link removal of symb/debug bulk.

## Worked example

**The situation.** `before.nm` is rich; `after.nm` is empty or errors; `./prog.stripped`
still works; size fell.

**Step 1 — baseline.** `nm` and `size` on the unstripped binary. The rejected wrong
reading is: "symbols are how the CPU finds `main` at runtime for a normal call." Calls
use addresses already fixed up in machine code / GOT; symbol *names* are metadata.

**Step 2 — strip the copy.** Never strip your only copy if you still need debug.
Compare. The rejected wrong reading is: "if `nm` fails, the binary cannot run." Running
tests execution; `nm` tests symbol presence.

**Step 3 — contrast exercise 38.** There, you revived debugging via addresses; here you
measure what strip deletes and that execution survives. Same family of facts, different
lab emphasis.

## Before/after capture recipe

```bash
cp prog prog.stripped
nm prog > before.nm
size prog > before.size
strip prog.stripped
nm prog.stripped > after.nm   # may error / be empty — that is data
size prog.stripped > after.size
./prog.stripped               # must still run
```

Contrast exercise 38 (revive debugging by address) — here you measure what strip deletes
and that execution survives.

## Distinctions worth keeping straight

- **Symtab vs machine code** — names versus bytes.
- **Strip vs visibility (50)** — post-link removal versus compile-time export policy.
- **Debug info vs dynamic symbols** — `-g` sections versus dynsym needs; inspect what
  *your* strip pass removed.
- **Size shrink vs speed** — fewer bytes on disk; not automatically faster CPU.

## Deeper worked navigation (strip still runs)

- strip removes metadata; code can remain.
- nm emptiness ≠ cannot run.
- copy before strip.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| nm before/after | rich → empty |
| size | often shrinks |
| ./stripped | still runs |

### Ordered navigation moves

1. Baseline nm/size.
2. strip copy.
3. rerun.
4. contrast ex38.

### Rejected wrong readings (keep beside the artifact)

- CPU finds main by symbol name.
- nm fail ⇒ binary dead.
- strip == visibility.

### Tool-line decoding reminders

- symtab vs .text
- size columns
- strip vs dynsym needs

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (52)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - nm empty ⇒ cannot run
- stripped only copy you needed to debug
- confused strip with visibility

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

1. Why can a binary run after `nm` becomes useless?
2. What should you compare besides `nm` to see strip's effect?
3. Why copy before stripping?
4. How does this lab's focus differ from exercise 38?

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

- Strip removes human/debugger metadata; loadable code can remain.
- Execution success and symbol richness are different axes.
- Always keep an unstripped build for debugging.
- `size` + `nm` before/after make the change concrete.

## Field-decoding recap for exercise 52

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 strip`, `man 1 nm`, `man 1 size`, `info binutils`

Now open `TASK.md` and do the practice.
