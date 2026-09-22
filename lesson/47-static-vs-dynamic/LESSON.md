# 47 — Static versus dynamic linking

The same `hello.c` can become a small binary that needs `libc.so` at runtime, or a
large binary that carries its own copy of library code. Those choices show up in
`ldd`, `size`, and `readelf -d`. This lesson trains you to *read* those artifacts so
"dynamic" and "static" stop being slogans.

## What this lesson asks of you

Build a tiny hello both ways (dynamic always; static or static-pie if the toolchain
allows). Fill a comparison: who has an interpreter / dynamic section, who is larger,
and why. If static link fails on this Arch box, document the linker error and go deep
on the dynamic binary anyway.

## Dynamic executables

Default `gcc hello.c -o hello` produces a dynamically linked ELF. `ldd` lists shared
libraries. `readelf -d` shows tags such as `NEEDED` (libraries) and often `INTERP`
(the dynamic linker path, also visible in the program headers). The on-disk file stays
small because libc's bytes live in a shared object mapped at runtime.

Calls to `printf` typically go through the Procedure Linkage Table (PLT) — Track E
exercise 48 zooms in. Here you only need to notice that dynamic machinery exists.

## Static and static-pie

`gcc -static` asks for a fully static link: libc archives are copied in. The binary
grows a lot; `ldd` should report that it is not a dynamic executable. `readelf -d` may
be empty or minimal — no `NEEDED` libc.

On some Arch setups, static glibc pieces are missing or awkward. Fallbacks:

- try `gcc -static-pie`
- if both fail, keep `hello.dyn`, record the error, and still answer every comparison
  question that dynamic artifacts can support

Static-pie is still "mostly self-contained" but may retain PIE dynamics — read the
headers you actually got rather than assuming the flag name.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` (dynamic) | `./hello` or `./hello.dyn` |
| static / static-pie target if provided | `./hello.static` or document linker error |
| `ldd ./hello.dyn` | lists libc + loader |
| `ldd ./hello.static` | "not a dynamic executable" (success) |
| `size` both | static much larger |
| `readelf -d` both | `NEEDED` present vs absent |

## Decoding `ldd` and `readelf -d`

| Artifact | Dynamic | Static |
| -------- | ------- | ------ |
| `ldd` | libc + ld-linux | not a dynamic executable |
| `readelf -d` | `NEEDED` libc | empty / no libc `NEEDED` |
| `size` | smaller text/data | much larger — copied library code |

**Rejected wrong reading:** "`ldd` failed, so the static binary is broken." For static,
that message is success.

**Rejected wrong reading:** "static is larger because of debug info." Dominant story is
copied library code.

## If `-static` fails on this Arch box

Document the linker error. Try `-static-pie` if offered. Still complete every comparison
question the dynamic binary can support — environment limits are not conceptual failure.

## Navigation anchors

- `ldd ./hello.dyn` — first dependency lines.
- `readelf -d ./hello.dyn` — search `NEEDED`.
- `readelf -h` — note type; compare to static-pie if you built it.

## Worked example

**The situation.** You have `hello.dyn` and, if lucky, `hello.static`. You run `ldd`,
`size`, and `readelf -d` on each.

**Step 1 — ldd.** Dynamic shows `libc.so.6` and a loader line. Static says "not a
dynamic executable" (or similar). The rejected wrong reading is: "ldd failed, so the
static binary is broken." For static binaries, that message is success.

**Step 2 — size.** Expect text/data/bss totals much larger for static. The rejected
wrong reading is: "static is larger because of debug info." Debug can add weight, but
the dominant story is *copied library code*.

**Step 3 — readelf -d.** Dynamic has `NEEDED` / `INTERP` related story; static lacks
libc `NEEDED`. Note what you see; cite `man 5 elf` for tag names when you need
spellings.

## Worked comparison triangle

1. **`ldd`** — dynamic lists libc; static says not dynamic (success).
2. **`size`** — static totals much larger; dominant story is copied library code, not
   "debug info."
3. **`readelf -d`** — dynamic has `NEEDED`; static lacks libc `NEEDED`.

Write those three observations side by side in your notes. If static link failed, write
the error and still complete every dynamic-side cell.

## `-static` versus `-static-pie`

Different ELF policies/types — inspect what you actually got (`readelf -h`) rather than
assuming the flag name.

## Distinctions worth keeping straight

- **Dynamic NEEDED vs static archive members** — runtime dependency versus bytes inside
  your ELF.
- **INTERP** — program interpreter for dynamic loading; static usually has none.
- **`-static` vs `-static-pie`** — different ELF types/policies; inspect, do not guess.
- **Link failure vs conceptual failure** — missing static libs are an environment limit,
  not a reason to skip reading the dynamic side.

## Deeper worked navigation (static vs dynamic)

- ldd/size/readelf-d triangle.
- Static larger mostly from copied lib code.
- Static link failure: document; finish dynamic cells.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| ldd dyn | libc listed |
| ldd static | not dynamic = success |
| size | static >> dynamic |
| readelf -d | NEEDED vs none |

### Ordered navigation moves

1. Build both (or document).
2. Fill triangle.
3. Note INTERP on dynamic.

### Rejected wrong readings (keep beside the artifact)

- ldd failure means static is broken.
- size gap is mostly debug info.
- DYN always means .so.

### Tool-line decoding reminders

- NEEDED tag
- INTERP/phdr interpreter
- size text/data/bss columns

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (47)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - thought ldd failure broke static
- blamed size on debug only
- skipped notes when static failed

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

1. Which artifact lists shared library dependencies for a dynamic hello?
2. Why is a static hello usually much larger on disk?
3. What `readelf -d` tags signal dynamic linking?
4. If `-static` fails here, what do you still owe in your notes?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Dynamic binaries defer library bytes to shared objects; static copies them in.
- `ldd`, `size`, and `readelf -d` are the quick triangle for comparison.
- Toolchain gaps may block static link; the mechanisms remain visible on the dynamic
  binary.
- File size differences are mostly policy, not mystery.

## Field-decoding recap for exercise 47

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 ldd`, `man 1 size`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`

Now open `TASK.md` and do the practice.
