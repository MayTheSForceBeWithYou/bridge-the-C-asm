# 39 — From faulting address to source line

When a process dies on a bad dereference, the useful artifact is often a
hexadecimal address in `%rip` / a backtrace frame — not a polite C diagnostic.
`addr2line` maps that address back through debug information (`-g` / `-ggdb`)
to a function name and source line. Stacks in nvim-dap and gdb `bt` already
*try* to show names; this lesson insists you also run `addr2line` yourself so
you can recover meaning when you only have an address (logs, core notes,
stripped-adjacent workflows). By the end you should extract a PC from Stacks /
`bt` / `%rip` and feed it to `addr2line` without treating the UI pretty-print as
a substitute for the skill.

## What this lesson asks of you

Keep `boom()` as a NULL deref, launch under dap or gdb, capture the faulting
address from Stacks / `bt` / `info registers rip`, then run:

```text
addr2line -e ./prog -f -C 0xADDR
```

and match it to `boom` / the offending line. Optionally strip a copy and see
the mapping degrade. Crashing is expected success.

`TASK.md` is practice. Do not "fix" the NULL deref — it is the input to the
tool.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | build `./prog` with `-ggdb` | debug info present for `addr2line` |
| (optional) strip a copy | remove debug/symbols | mapping degrades — contrast with lesson 38 |

## What `addr2line` needs

The tool reads the debug info embedded (or linked) with the ELF you pass via
`-e`. Flags you want here:

| Flag | Role |
| ---- | ---- |
| `-e ./prog` | ELF to read debug info from (must be the binary you ran, or its unstripped twin with matching build-id/layout) |
| `-f` | Print function name |
| `-C` | Demangle (C mostly unchanged; harmless) |
| `0xADDR` | The faulting address you captured |

Without `-g` / `-ggdb` in the build, addresses stay numbers. After `strip`,
debug sections are often gone — `addr2line` then shrugs even though the crash
address is real.

### Where to read `0xADDR` (recognition)

| Source | What to copy | Pitfall |
| ------ | ------------ | ------- |
| dap **Stacks** pane | Code address on the faulting frame | Do not copy a frame *index* or a library slot you did not mean |
| gdb `bt` | Address shown for the top / faulting frame | Skip frames that are signal trampolines if a clearer `boom` frame exists |
| `info registers rip` / Scopes regs | Value of `%rip` at the stop | Confirm you stopped on the faulting insn, not after unwind weirdness |
| Lesson 37-style smash PC | Often `0x41414141…` nonsense | `addr2line` cannot map fiction — contrast with this clean NULL deref |

**Recognition rule:** the PC is a *code* address (instruction address class),
not a stack slot address and not the NULL pointer you dereferenced (`0x0` is
the *data* address that faulted; `%rip` is where the faulting *load* lived).

PIE/ASLR means runtime addresses may need care relative to load base. For this
track's small `./prog` under gdb/dap, the address shown in `bt` / Stacks for the
faulting frame is usually the one `addr2line -e ./prog` accepts. If results look
like `??:0`, verify you used the same binary path you ran and that `-ggdb` was
on.

## Stacks / `bt` versus `addr2line`

dap Stacks and gdb `bt` are interactive unwind views. `addr2line` is a batch
mapper from address → line, the same family of skill you need for crash tickets
that only quote a PC. The TASK requires `addr2line` even if Stacks already
looked obvious — do not skip it.

**Rejected wrong reading:** "Stacks already says `boom`, so I am done." You
still run `addr2line` for this lab's skill.

Inlining and optimization can make the reported line surprise you (fault
attributed to a nearby line or caller). At default debug `-O0` for this
exercise, expect a clean map to `boom`.

## Keys and session shape

| Step | nvim-dap | gdb |
| ---- | -------- | --- |
| Optional pre-break | F9 on `boom` | `break boom` |
| Launch | Launch existing → `./prog` | `run` |
| On fault | Read Stacks address | `bt` / `info registers rip` |
| Map | shell: `addr2line -e ./prog -f -C 0xADDR` | same |
| End | `<leader>dt` | `quit` |

## Worked example

**The situation.** `boom` does `*p` with `p == NULL`. You launch `./prog`.

**Step 1 — catch the crash.** Optional F9 on `boom` to inspect before the deref;
or run with no breakpoint and let the session stop on the signal. Stacks / `bt`
name a frame; note the **code** address. Confirm you did not copy `0x0` thinking
that was the PC.

**Step 2 — map.** `addr2line -e ./prog -f -C 0xADDR` should report `boom` and a
line in `prog.c`. Compare to the Stacks line. Small differences can come from
how the compiler attributed the load instruction.

**Step 3 — optional strip contrast.** Strip a copy, crash again if you can still
get an address from `bt`, re-run `addr2line` on the stripped file — expect loss
of mapping. That pairs with lesson 38's "symbols are convenience."


## Reading Stacks / `bt` without grabbing the wrong hex

A Stacks pane row often looks like: frame index, function name, path:line, and
a code address. Juniors sometimes copy the frame number, a data address from
Locals (`p == 0x0`), or a libc frame above/below `boom`.

**Navigation rule:** identify the frame whose function is `boom` (or the frame
gdb marks as the fault site). Copy the **code address** attached to that frame.
If only `%rip` is trustworthy, use that when you are stopped on the faulting
instruction.

Example shape of a good shell line once you have the address:

```text
addr2line -e ./prog -f -C 0x5555555551a6
```

Good output names `boom` and a `prog.c` line. `??:0` or `?? ??:0` means debug
info missing, wrong ELF, or wrong address class — debug the *inputs* to
`addr2line`, not the crash itself.

### Optional core / log fantasy (why the skill exists)

Imagine a teammate pastes only `PC=0x5555555551a6` into chat. Stacks will not
be open on their machine. `addr2line -e path/to/matching/binary -f -C` is how
you recover. That is why the TASK forbids stopping when Stacks already looks
obvious.

**Rejected wrong reading:** "Pretty Stacks made addr2line obsolete." UI and
batch mapping are different durability levels for the same underlying debug
info.



## Minimal good notes for your lab log

Record four lines: (1) how you obtained `0xADDR`, (2) the exact `addr2line`
command, (3) its output, (4) the matching source line you opened. That artifact
trail is what crash tickets look like. "Stacks said boom" is not enough for
this TASK's standard.



## Build-id / matching binary discipline

`addr2line -e` must point at an ELF that still carries the debug info matching
the addresses you captured. If you rebuild between crashing and mapping,
addresses can shift under PIE layouts. Prefer: build once, crash, map, *then*
optional strip experiments on a copy. Mixing `./prog` from build A with
addresses from build B is a classic `??:0` self-own.


## Distinctions worth keeping straight

- **Runtime PC vs source line** — hex versus file:line via debug info.
- **Faulting data address (`NULL`) vs faulting PC (`%rip`)** — what you touched
  versus which instruction touched it.
- **Stacks convenience vs `addr2line` skill** — UI versus portable mapping.
- **`-ggdb` binary vs stripped twin** — mapping works only with debug data.
- **Crash as failure vs crash as input** — here the crash feeds the tool.
- **Clean NULL deref PC vs smashed `0x41…` PC** — mappable versus nonsense.

## Check yourself

1. Which compiler/debug feature must the binary have for `addr2line` to name
   `boom`?
2. Why does this TASK still require `addr2line` if dap Stacks already shows a
   function name?
3. What happens to `addr2line` quality after `strip`?
4. Where can you read the faulting address besides Stacks?
5. You accidentally pass `0x0` to `addr2line`. What conceptual mix-up is that?
6. Why might `addr2line` on a lesson-37 smash PC return `??` even with `-ggdb`?

If fuzzy, re-read the "where to read `0xADDR`" table before `man 1 addr2line`.

## Key takeaways

- Crash addresses become actionable with `addr2line -e BINARY -f -C 0xADDR`
  when debug info exists.
- Practice extracting the PC from Stacks / `bt` / `%rip`, not only reading
  prettified frames.
- The PC is not the NULL pointer you dereferenced — it is where the faulting
  instruction lived.
- Strip and missing `-g` are common reasons mapping returns `??`.
- Intentional NULL deref here is a controlled crash for tooling practice.

## Lookup (not the lesson)

- `NVIM_DAP.md` — Stacks after crash; addr2line note
- `man 1 addr2line`, `man 1 gdb` (`help bt`), `man 1 strip`

Now open `TASK.md` and do the practice.
