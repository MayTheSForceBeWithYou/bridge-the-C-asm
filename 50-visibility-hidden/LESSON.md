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

## Distinctions worth keeping straight

- **Static symtab (`nm`) vs dynsyms** — link-time names versus runtime export surface.
- **Hidden default vs explicit default attribute** — policy versus per-symbol override.
- **Executable vs shared library stakes** — both inspectable; DSO API design cares more.
- **Strip vs visibility** — strip removes debug/symtab bulk; visibility changes export
  policy at compile/link time (see also exercise 52).

## Check yourself

1. Which tool shows the dynamic export table specifically?
2. Does `-fvisibility=hidden` delete functions from the binary's text?
3. How do you export one API while keeping a hidden default?
4. Why do shared libraries care more about this flag?

## Key takeaways

- Visibility controls dynamic export, not whether code exists in `.text`.
- `-fvisibility=hidden` flips the default to hidden.
- Compare `nm` with `readelf --dyn-syms` to avoid confusing the two tables.
- DSO API hygiene is the practical payoff; executables still demonstrate the mechanism.

## Lookup (not the lesson)

- `man 1 nm`, `man 1 readelf`, `man 1 gcc`, `info gcc`

Now open `TASK.md` and do the practice.
