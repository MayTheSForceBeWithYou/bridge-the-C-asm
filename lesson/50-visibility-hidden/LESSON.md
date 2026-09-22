# 50 — Default visibility versus hidden

ELF symbols can be exported for dynamic linking or hidden from the dynamic symbol
table. GCC's `-fvisibility=hidden` changes the default so only explicitly default-visible
symbols remain dynamically exported. This lesson shows the effect with `nm` and
`readelf --dyn-syms` on an ordinary program — and asks you to reason about how the
same flag matters even more for shared libraries.

## What this lesson asks of you

Build `prog` default and `prog.hidden` with `-fvisibility=hidden`. Diff `nm` and
dynamic symbol tables. Document which functions remain exported and what visibility
means for an executable versus a `.so`.

## Visibility in practice

Without the flag, global functions are typically dynamically visible (subject to link
mode). With `-fvisibility=hidden`, the compiler marks symbols hidden unless you
annotate `__attribute__((visibility("default")))` on APIs you intend to export.

For a main executable, fewer dynsyms may show up; the program still runs. For a shared
library, hidden defaults prevent leaking internal APIs across the DSO boundary and can
enable better optimization (fewer preemptible symbols).

`nm` shows local/global binding in the static symbol table; `readelf --dyn-syms` shows
what the dynamic linker knows about. Both views matter.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | `./prog` default visibility |
| hidden build target | `./prog.hidden` with `-fvisibility=hidden` |
| `nm ./prog` vs `nm ./prog.hidden` | static symtab still populated |
| `readelf --dyn-syms` both | dynamic export surface shrinks when hidden |

## Two tables, two stories

| Tool | Table | Question it answers |
| ---- | ----- | ------------------- |
| `nm` | static symbol table | what names exist for linking/debug |
| `readelf --dyn-syms` | dynamic symbols | what the loader/export surface sees |

**Rejected wrong reading:** "hidden means `nm` prints nothing." Static `nm` can still be
rich; *dynsyms* shrink.

**Rejected wrong reading:** "visibility only matters for `.so` files, so comparing two
executables is meaningless." You still see dynsym differences; the *motivation* is
strongest for DSOs — say that in your notes.

## Exporting one API under a hidden default

`__attribute__((visibility("default")))` on the symbols you intend to export.

## Strip vs visibility (preview of 52)

Strip removes symbol/debug bulk after link. Visibility changes export *policy* at
compile/link time. Different knobs.

## Worked example

**The situation.** `readelf --dyn-syms ./prog.hidden` is thinner than `./prog` for your
internal helpers.

**Step 1 — build both.** Same source, different visibility flag. The rejected wrong
reading is: "hidden means `nm` prints nothing at all." Static `nm` still lists many
symbols; *dynamic* export is what shrinks.

**Step 2 — compare dynsyms.** Note which names survive. The rejected wrong reading is:
"visibility only affects `.so` files, so an executable comparison is meaningless." You
still see dynsym differences; the *motivation* is strongest for DSOs, which you should
state in notes.

**Step 3 — reason.** Hidden internals cannot be interposed as easily; default exports
are the public surface.

## Worked comparison steps

1. Build default and `-fvisibility=hidden` variants from the same source.
2. Diff `readelf --dyn-syms` — internal helpers should vanish from the hidden build's
   dynamic table.
3. Note `nm` still lists many names — static vs dynamic tables.
4. Write one paragraph: strongest motivation is DSO API hygiene; executables still
   demonstrate the mechanism.

**Rejected wrong reading:** "hidden deletes functions from `.text`." Visibility changes
export policy, not whether code exists.

## Distinctions worth keeping straight

- **Static symtab (`nm`) vs dynsyms** — link-time names versus runtime export surface.
- **Hidden default vs explicit default attribute** — policy versus per-symbol override.
- **Executable vs shared library stakes** — both inspectable; DSO API design cares more.
- **Strip vs visibility** — strip removes debug/symtab bulk; visibility changes export
  policy at compile/link time (see also exercise 52).

## Deeper worked navigation (visibility)

- hidden default shrinks dynsyms.
- nm static table can stay rich.
- DSO API hygiene is the main motivation.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| readelf --dyn-syms | fewer exports when hidden |
| nm | still has names |
| notes | executable demo vs .so stakes |

### Ordered navigation moves

1. Build both.
2. Diff dynsyms.
3. Write motivation paragraph.

### Rejected wrong readings (keep beside the artifact)

- hidden deletes .text.
- nm empty means success.
- executables make the exercise meaningless.

### Tool-line decoding reminders

- static symtab vs dynsyms
- default attribute override
- strip vs visibility

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (50)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - hidden deletes .text
- confused nm with dynsyms
- thought exec demos are worthless

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

1. Which tool shows the dynamic export table specifically?
2. Does `-fvisibility=hidden` delete functions from the binary's text?
3. How do you export one API while keeping a hidden default?
4. Why do shared libraries care more about this flag?

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

- Visibility controls dynamic export, not whether code exists in `.text`.
- `-fvisibility=hidden` flips the default to hidden.
- Compare `nm` with `readelf --dyn-syms` to avoid confusing the two tables.
- DSO API hygiene is the practical payoff; executables still demonstrate the mechanism.

## Field-decoding recap for exercise 50

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 nm`, `man 1 readelf`, `man 1 gcc`, `info gcc`

Now open `TASK.md` and do the practice.
