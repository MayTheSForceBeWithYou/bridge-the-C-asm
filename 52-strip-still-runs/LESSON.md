# 52 — Stripping symbols from a running binary
<!-- concepts: c.p5.elf.strip -->

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

## Distinctions worth keeping straight

- **Symtab vs machine code** — names versus bytes.
- **Strip vs visibility (50)** — post-link removal versus compile-time export policy.
- **Debug info vs dynamic symbols** — `-g` sections versus dynsym needs; inspect what
  *your* strip pass removed.
- **Size shrink vs speed** — fewer bytes on disk; not automatically faster CPU.

## Check yourself

1. Why can a binary run after `nm` becomes useless?
2. What should you compare besides `nm` to see strip's effect?
3. Why copy before stripping?
4. How does this lab's focus differ from exercise 38?

## Key takeaways

- Strip removes human/debugger metadata; loadable code can remain.
- Execution success and symbol richness are different axes.
- Always keep an unstripped build for debugging.
- `size` + `nm` before/after make the change concrete.

## Lookup (not the lesson)

- `man 1 strip`, `man 1 nm`, `man 1 size`, `info binutils`

Now open `TASK.md` and do the practice.
