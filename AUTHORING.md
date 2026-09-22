# Authoring guide — bridge-the-C-asm

Adapted from Otto Didact `docs/content-authoring-guide.md` for a **lab track** (C ↔
x86-64 asm). Otto modules teach certification judgement; these lessons teach you to
*read machines*. Same pedagogy, different artifact.

**Canonical depth bar:** `lesson/01-hello-pipeline/LESSON.md` (especially the sections that
decode `objdump` columns and tell the reader exactly what to search for). When another
lesson feels "thin," raise it toward that bar — do not raise `TASK.md` into a second
lecture.

## Split of responsibilities

| File | Job |
| ---- | --- |
| `LESSON.md` | Teach. Advance organizer, foundations, worked example, distinctions, check, takeaways. |
| `TASK.md` | Practice only. Commands to run, what to implement, how you know you are done. |
| `REFS.md` / man / info | Lookup — flag names, struct field names, syscall numbers. **Not the teacher.** |

If a `TASK.md` says "read `man 1 gcc`" as the way to learn a concept, that task is wrong.
Teach the concept in `LESSON.md`; link the man page under **Lookup** for spelling.

## Lesson template (headings are exact)

```text
# NN — <title>

<2–4 sentence advance organizer. Do not restate the title.>

## What this lesson asks of you
## <named foundation sections — as many as needed; never "Part 1">
## Worked example
## Distinctions worth keeping straight
## Check yourself
## Key takeaways
## Lookup (not the lesson)
```

End the lesson by sending the reader to `TASK.md`.

## Depth and hand-holding (LESSON side) — load-bearing rules

These rules exist because early drafts pointed at tools without teaching people how to
*see* what the tools printed. Fix that in `LESSON.md`, not in chat, and not by dumping
man pages.

1. **If you tell the reader to "note" or "find" something, teach the recognition rule.**
   Bad: "Note the instruction address." Good: name the column, show a real line, and say
   which token is the address versus an operand versus a `#` annotation.
2. **Decode tool output as columns/fields, not as a blob of hex.** Whenever `objdump`,
   `readelf`, `nm`, `gdb`/`nvim-dap`, or similar prints several hex values on one line,
   add a short table or bullet list: what each field *is*. Anticipate the mix-up (RIP
   offset vs address was the 01 failure mode).
3. **Give a navigation move before interpretation.** Prefer "search for `<main>:`" /
   "scroll to the `.rodata` label" / "break on this symbol" over "look at the listing."
   Readers drown in CRT, PLT, and headers when you skip the anchor.
4. **Show the shape of a real line from this track's toolchain.** Prefer an example that
   matches GNU `as` AT&T + `objdump -d` on x86-64 Linux here. Numbers may vary with PIE
   layout; say so explicitly, and stress that *column meaning* does not.
5. **Name the rejected wrong reading next to the tricky artifact.** Competent juniors
   confuse `.s` with something runnable, `.o` with an executable, `.lst` with a new build
   stage, and the first `call` in a file with "my printf." Write those mistakes down and
   correct them.
6. **Map Makefile targets to commands and to files to open.** Do not assume the reader
   remembers that `disasm` only saves `objdump -d` output.
7. **Keep man/info under Lookup.** The lesson teaches *what to look for* and *how to
   recognize it*. Man pages supply flag spelling and struct field names after that.
8. **Hand-holding belongs in `LESSON.md`; restraint belongs in `TASK.md`.** Extensive
   description, worked navigation, and column decoding are appropriate in the lesson.
   The task should stay lean: do the lab, use the lesson as the guide, verify with `ok` /
   exit code / named observations — not a second copy of the lecture.

### Voice

- Second person ("you"). Instructor-clear, not cute, not corporate.
- Explain mechanisms in prose. Fenced commands are fine; **load-bearing facts must also
  appear in prose** (so a skimmer who skips fences still learns).
- Every worked example includes **one rejected wrong reading** — the mistake a competent
  junior makes — as a paragraph, not a heading.
- Do **not** dump full man pages. Quote a flag only when the lesson needs that flag.
- Do **not** give solution bodies for student stubs. Worked examples interpret *artifacts*
  (asm shape, register story, ELF headers), not finished `TODO` implementations.
- US spelling. Expand uncommon acronyms once (PLT, ABI, ELF, PIE, CRT).

### "A bit less hand-holding" (practice side only)

- `TASK.md` should not narrate every keystroke already covered in the lesson.
- Leave implementation, observation, and judgement to the student.
- Success criteria should be checkable (`ok`, exit code, or named observations) without an
  answer key file.
- If students stall because the *lesson* omitted a recognition rule, fix the lesson — do
  not compensate by pasting lecture prose into `TASK.md`.

### Check yourself

Production prompts answerable from the lesson alone. Include at least one item that checks
a recognition rule (columns, labels, anchors), not only vocabulary recall. If the only way
to answer is to have memorized an external man page, the lesson failed.

### Key takeaways

Defensible claims (3–6 bullets), not a restatement of the headings.

## When revising an existing lesson

Use this checklist (inspired by the 01 `objdump` confusion):

- [ ] Every "inspect / note / find" sentence has a recognition rule next to it
- [ ] Multi-field tool output is decoded field-by-field
- [ ] A search/anchor instruction appears before a busy listing
- [ ] At least one rejected wrong reading matches a real junior mix-up for this artifact
- [ ] Makefile / command → output file mapping is explicit when targets are non-obvious
- [ ] Lookup lists only verified local docs; no fake man pages
- [ ] `TASK.md` stayed practice-only after the lesson got deeper

## README / philosophy

Lead with: learn from `LESSON.md`, prove it in `TASK.md`, use man/info as reference.
Reps still matter; untaught reps do not. Deep lessons beat shallow prompts that outsource
teaching to `man`.
