# 49 — PIE, EXEC, and ASLR bases

Modern GCC defaults to position-independent executables (PIE). In `readelf -h`, that
often shows as `Type: DYN` — the same ELF type family as shared objects — whereas
`-no-pie` yields `Type: EXEC` with a preferred load address. Address Space Layout
Randomization (ASLR) then slides PIE mappings between runs. This lesson connects those
header bits to what you see in gdb for `$rip`.

## What this lesson asks of you

Build default PIE and `-no-pie` variants. Record `Type:` for each. In gdb on the PIE
binary, break at `main`, run twice, and compare `$rip` / mappings with randomization on
versus `set disable-randomization on`.

## DYN versus EXEC

- **PIE (`Type: DYN`)**: loadable at a random base; code typically uses RIP-relative
  addressing.
- **`-no-pie` (`Type: EXEC`)**: classic executable type with a linked-in preferred
  address; ASLR behavior differs (some systems still randomize some regions).

Do not equate "DYN" with "this is a `.so`." Your main program can be DYN and still be
the executable you run.

## Observing ASLR in gdb

gdb may disable ASLR by default for reproducibility (`disable-randomization`). For this
exercise you want to *see* movement: ensure randomization is enabled, `run` to `main`,
record `$rip` or text mapping base, `kill`, `run` again, compare. Then turn
`disable-randomization on` and show stability.

Absolute addresses in notes are ephemeral; what matters is *whether they changed*.

## Worked example

**The situation.** `readelf -h ./prog | grep Type` says `DYN`; `./prog.nopie` says
`EXEC`. Two gdb runs with ASLR enabled show different `$rip` values at the same `main`
breakpoint source line.

**Step 1 — headers.** Confirm types. The rejected wrong reading is: "`Type: DYN` means
I accidentally built a shared library instead of a program." PIE executables report DYN.

**Step 2 — ASLR on.** Note `$rip` changes between runs (usually). The rejected wrong
reading is: "ASLR is broken because gdb showed the same address twice." Check whether
gdb disabled randomization; the lesson's point includes that knob.

**Step 3 — ASLR off in gdb.** Addresses stabilize. That contrast is the evidence.

## Distinctions worth keeping straight

- **PIE DYN vs shared object DYN** — same type field family, different roles.
- **EXEC preferred address vs relocated PIE** — fixed link address versus slide.
- **gdb disable-randomization** — debugger policy, not a kernel mystery.
- **Text base vs stack/heap bases** — ASLR randomizes multiple regions; watch the one
  you claim to watch.

## Check yourself

1. What `readelf -h` type do you expect for default PIE? For `-no-pie`?
2. Why might two gdb runs show identical `$rip` even on a PIE binary?
3. What command toggles gdb's randomization behavior?
4. Does `Type: DYN` alone prove the file is a `.so`?

## Key takeaways

- PIE binaries often advertise `Type: DYN`; `-no-pie` yields `EXEC`.
- ASLR slides PIE bases across process starts.
- gdb can mask ASLR unless you enable randomization.
- Header type + live `$rip` together tell the deployment story.

## Lookup (not the lesson)

- `man 1 readelf`, `man 5 elf`, `man 1 gdb`, `man 1 gcc`

Now open `TASK.md` and do the practice.
