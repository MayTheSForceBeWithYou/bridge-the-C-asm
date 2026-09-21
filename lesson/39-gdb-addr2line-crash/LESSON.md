# 39 — From faulting address to source line

When a process dies on a bad dereference, the useful artifact is often a
hexadecimal address in `%rip` / a backtrace frame — not a polite C diagnostic.
`addr2line` maps that address back through debug information (`-g` / `-ggdb`)
to a function name and source line. Stacks in nvim-dap and gdb `bt` already
*try* to show names; this lesson insists you also run `addr2line` yourself so
you can recover meaning when you only have an address (logs, core notes,
stripped-adjacent workflows).

## What this lesson asks of you

Keep `boom()` as a NULL deref, launch under dap or gdb, capture the faulting
address from Stacks / `bt` / `info registers rip`, then run:

```text
addr2line -e ./prog -f -C 0xADDR
```

and match it to `boom` / the offending line. Optionally strip a copy and see
the mapping degrade. Crashing is expected success.

## What `addr2line` needs

The tool reads the debug info embedded (or linked) with the ELF you pass via
`-e`. Flags you want here: `-f` for function name, `-C` to demangle if needed
(C mostly unchanged). Without `-g` in the build, addresses stay numbers. After
`strip`, debug sections are often gone — `addr2line` then shrugs even though
the crash address is real.

PIE/ASLR means runtime addresses may need care relative to load base. For this
track's small `./prog` under gdb/dap, the address shown in `bt` / Stacks for
the faulting frame is usually the one `addr2line -e ./prog` accepts. If results
look like `??:0`, verify you used the same binary path you ran and that `-ggdb`
was on.

## Stacks / `bt` versus `addr2line`

dap Stacks and gdb `bt` are interactive unwind views. `addr2line` is a
batch mapper from address → line, the same family of skill you need for crash
tickets that only quote a PC. The TASK requires `addr2line` even if Stacks
already looked obvious — do not skip it.

Inlining and optimization can make the reported line surprise you (fault
attributed to a nearby line or caller). At default debug `-O0` for this
exercise, expect a clean map to `boom`.

## Worked example

**The situation.** `boom` does `*p` with `p == NULL`. You launch `./prog`.

**Step 1 — catch the crash.** Optional F9 on `boom` to inspect before the
deref; or run with no breakpoint and let the session stop on the signal.
Stacks / `bt` name a frame; note the code address. The rejected wrong reading
is: "Stacks already says `boom`, so I am done." You still run `addr2line` for
this lab's skill.

**Step 2 — map.** `addr2line -e ./prog -f -C 0xADDR` should report `boom` and a
line in `prog.c`. Compare to the Stacks line. Small differences can come from
how the compiler attributed the load instruction.

**Step 3 — optional strip contrast.** Strip a copy, crash again if you can still
get an address from `bt`, re-run `addr2line` on the stripped file — expect loss
of mapping. That pairs with lesson 38's "symbols are convenience."

## Distinctions worth keeping straight

- **Runtime PC vs source line** — hex versus file:line via debug info.
- **Stacks convenience vs `addr2line` skill** — UI versus portable mapping.
- **`-ggdb` binary vs stripped twin** — mapping works only with debug data.
- **Crash as failure vs crash as input** — here the crash feeds the tool.

## Check yourself

1. Which compiler/debug feature must the binary have for `addr2line` to name
   `boom`?
2. Why does this TASK still require `addr2line` if dap Stacks already shows a
   function name?
3. What happens to `addr2line` quality after `strip`?
4. Where can you read the faulting address besides Stacks?

## Key takeaways

- Crash addresses become actionable with `addr2line -e BINARY -f -C 0xADDR`
  when debug info exists.
- Practice extracting the PC from Stacks / `bt` / `%rip`, not only reading
  prettified frames.
- Strip and missing `-g` are common reasons mapping returns `??`.
- Intentional NULL deref here is a controlled crash for tooling practice.

## Lookup (not the lesson)

- `NVIM_DAP.md` — Stacks after crash; addr2line note
- `man 1 addr2line`, `man 1 gdb` (`help bt`), `man 1 strip`

Now open `TASK.md` and do the practice.
