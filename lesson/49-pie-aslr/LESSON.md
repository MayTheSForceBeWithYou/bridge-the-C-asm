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

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | PIE `./prog` |
| no-pie target | `./prog.nopie` |
| `readelf -h ./prog \| grep Type` | `DYN` for PIE |
| `readelf -h ./prog.nopie \| grep Type` | `EXEC` for `-no-pie` |
| gdb with ASLR on/off | `$rip` / mapping base movement |

## Decoding `Type:` without panicking

| `Type:` | Meaning here |
| ------- | ------------ |
| `DYN` | often PIE executable (or a `.so`) |
| `EXEC` | classic non-PIE executable |

**Rejected wrong reading:** `Type: DYN` means you accidentally built a shared library.
PIE mains commonly report `DYN`.

## gdb randomization knob (recognition rule)

gdb may disable ASLR by default. For this lab:

1. `set disable-randomization off`
2. `break main` → `run` → record `$rip` or text base (`info proc mappings`)
3. `kill` → `run` again → compare
4. `set disable-randomization on` → show stability

**Rejected wrong reading:** "ASLR is broken because two gdb runs matched" without checking
the knob.

## What must stay constant in your notes

Absolute addresses are ephemeral. Record *whether* they changed — not that a particular
hex is sacred.

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

## Worked ASLR evidence checklist

| Step | What to record |
| ---- | -------------- |
| `readelf -h` PIE | `Type: DYN` |
| `readelf -h` no-pie | `Type: EXEC` |
| gdb ASLR on, run #1 | `$rip` or text base |
| gdb ASLR on, run #2 | different base (usually) |
| gdb ASLR off | stable bases |

Absolute hex is ephemeral; *whether it moved* is the lesson.

## Multiple randomized regions

ASLR can slide text, stack, and libs differently. Watch the region you claim to watch
(`$rip` at `main` vs stack address).

## Distinctions worth keeping straight

- **PIE DYN vs shared object DYN** — same type field family, different roles.
- **EXEC preferred address vs relocated PIE** — fixed link address versus slide.
- **gdb disable-randomization** — debugger policy, not a kernel mystery.
- **Text base vs stack/heap bases** — ASLR randomizes multiple regions; watch the one
  you claim to watch.

## Deeper worked navigation (PIE and ASLR)

- PIE often Type DYN; -no-pie EXEC.
- gdb may disable ASLR — flip the knob.
- Record whether addresses moved.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| readelf -h PIE | Type DYN |
| readelf -h nopie | Type EXEC |
| gdb two runs | rip/base move when ASLR on |

### Ordered navigation moves

1. Compare Types.
2. gdb ASLR on: two runs.
3. gdb ASLR off: stable.

### Rejected wrong readings (keep beside the artifact)

- DYN means accidental .so.
- Matched gdb runs prove ASLR broken (ignore the knob).

### Tool-line decoding reminders

- Type field
- disable-randomization
- text vs stack bases

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (49)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - DYN means accidental .so
- ignored gdb ASLR knob
- recorded absolute hex as eternal

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

1. What `readelf -h` type do you expect for default PIE? For `-no-pie`?
2. Why might two gdb runs show identical `$rip` even on a PIE binary?
3. What command toggles gdb's randomization behavior?
4. Does `Type: DYN` alone prove the file is a `.so`?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- PIE binaries often advertise `Type: DYN`; `-no-pie` yields `EXEC`.
- ASLR slides PIE bases across process starts.
- gdb can mask ASLR unless you enable randomization.
- Header type + live `$rip` together tell the deployment story.

## Field-decoding recap for exercise 49

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 readelf`, `man 5 elf`, `man 1 gdb`, `man 1 gcc`

Now open `TASK.md` and do the practice.
