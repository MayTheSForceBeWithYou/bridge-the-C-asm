# 48 — PLT stubs and lazy binding

When a dynamic binary calls `printf`, the call instruction usually targets a stub in
the Procedure Linkage Table (PLT), not the final libc address. The first call asks the
dynamic linker to resolve the symbol and rewrites a Global Offset Table (GOT) entry so
later calls jump straight there. This lesson teaches you to *see* that stub and name the
mechanism.

## What this lesson asks of you

Build a dynamic hello that calls `printf`. With `objdump -d -j .plt` and `readelf`,
locate `printf@plt`, describe what it jumps through, and explain lazy binding in your
own notes. Optional gdb (break, call twice) makes first-vs-later vivid.

## PLT and GOT in one paragraph

The PLT stub is a tiny reachable trampoline in your binary. It loads or jumps via a GOT
slot that initially points back into the resolver path. On first use, the dynamic linker
looks up `printf` in libc and updates the GOT. Lazy binding means "resolve on first
call," which is why the first invocation can look different under the debugger than the
second.

`readelf -r` shows relocations related to this; `readelf -d` shows dynamic tags. You do
not need every relocation type memorized — you need the call story.

## How to look

```
objdump -d -j .plt ./hello
objdump -d ./hello | less   # find call toward printf@plt
readelf -r ./hello
readelf -d ./hello
```

In `main`, expect `call printf@plt` (AT&T spelling may vary slightly). In `.plt`, expect
a short sequence ending in a jump through a GOT-related address.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | dynamic `./hello` |
| `objdump -d -j .plt ./hello` | `printf@plt` stub |
| `objdump -d ./hello` | search `<main>:`; find `call … @plt` |
| `readelf -r ./hello` | relocations for binding |
| `readelf -d ./hello` | dynamic tags |
| optional gdb | first vs later call |

## Decoding a `call` to the PLT

```text
    1179:	e8 d2 fe ff ff       	call   1050 <printf@plt>
```

| Column | Example | Meaning |
| ------ | ------- | ------- |
| Instruction address | `1179:` | where the `call` lives |
| Bytes | `e8 …` | relative call encoding |
| Operand hint | `1050 <printf@plt>` | PLT stub in *your* text |

**Rejected wrong reading:** the `call` target is already the final libc VA baked at link
time. Lazy binding targets the PLT stub; ASLR moves libc anyway.

## Reading the `.plt` stub

**Navigation:** `objdump -d -j .plt ./hello` then search `printf`. Expect a short
trampoline that ends jumping via a GOT-related slot. First call may bounce through the
resolver; later calls use the filled GOT.

**Rejected wrong reading:** "The GOT is only for global data variables." Function symbols
use GOT slots for PLT binding too.

## Lazy vs `LD_BIND_NOW`

Lazy: resolve on first call. `LD_BIND_NOW=1` resolves at startup — first-call path looks
different under gdb/time.

## Worked example

**The situation.** You found the address of `printf@plt` and wrote: first call resolves;
later calls use the filled GOT.

**Step 1 — find the stub.** `objdump -d -j .plt` lists stubs. Match the one named for
`printf`. The rejected wrong reading is: "the address in `call` is already the final
libc address baked at link time." For dynamic lazy binding, the call targets the PLT
stub in *your* text, not the ultimate libc VA (which moves with ASLR anyway).

**Step 2 — name the indirection.** The stub uses the GOT. The rejected wrong reading is:
"the GOT is only for global data variables." Function symbols use GOT slots too for PLT
binding.

**Step 3 — optional gdb.** `break printf`, `run`, dig around, `finish`, call again —
observe whether you still stop the same way. Illuminating, not required for credit if
you already explained lazy binding from the dump.

## Worked path through the artifacts

1. `objdump -d -j .plt ./hello` — list stubs; match `printf`.
2. From `<main>:`, find `call … <printf@plt>`; note instruction address vs stub name.
3. Explain: first call may resolve; GOT updates; later calls hit libc through the filled
   slot.
4. Optional gdb: break, call twice, observe first-vs-later.

**Rejected wrong reading:** "static hello will show the same `printf@plt` story." Static
links often call more directly — contrast with 47.

## Relocation records vs runtime addresses

`readelf -r` shows metadata; the filled GOT after resolve is the runtime address. Do not
memorize every reloc type — keep the call story.

## Distinctions worth keeping straight

- **PLT stub vs final function body** — trampoline versus libc implementation.
- **Lazy vs now binding (`LD_BIND_NOW`)** — resolve on first call vs at startup.
- **Relocation records vs runtime addresses** — metadata versus filled GOT after
  resolve.
- **Static binary** — often no printf PLT; the call may be direct. Contrast with 47.

## Deeper worked navigation (PLT lazy binding)

- call targets PLT stub in your text.
- GOT fills on first resolve.
- GOT used for functions too.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| objdump -j .plt | printf@plt stub |
| call in main | operand names @plt |
| optional gdb | first vs later call |

### Ordered navigation moves

1. Dump .plt.
2. Find call in main.
3. Explain lazy binding.
4. Optional LD_BIND_NOW contrast.

### Rejected wrong readings (keep beside the artifact)

- call already absolute libc VA.
- GOT is data-only.
- static binaries show same PLT story.

### Tool-line decoding reminders

- call columns: addr|bytes|target hint
- reloc metadata vs runtime GOT
- lazy vs now

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (48)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - thought call target was final libc VA
- GOT is data-only myth
- ignored lazy vs BIND_NOW

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

1. Why does `main` call `printf@plt` instead of an absolute libc address?
2. What gets updated on the first call under lazy binding?
3. Which `objdump` flag limits output to the PLT section?
4. How would a fully static hello differ in this inspection?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Dynamic calls to shared functions commonly go through the PLT.
- Lazy binding resolves on first use via the GOT and dynamic linker.
- `objdump -j .plt` and `readelf` make the mechanism inspectable.
- First call and later calls can differ in observable cost/path.

## Field-decoding recap for exercise 48

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 objdump`, `man 1 readelf`, `man 5 elf`, `info ld`, `man 1 gdb`

Now open `TASK.md` and do the practice.
