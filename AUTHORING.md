# Authoring guide — bridge-the-C-asm

Adapted from Otto Didact `docs/content-authoring-guide.md` for a **lab track** (C ↔
x86-64 asm). Otto modules teach certification judgement; these lessons teach you to
*read machines*. Same pedagogy, different artifact.

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

### Voice

- Second person ("you"). Instructor-clear, not cute, not corporate.
- Explain mechanisms in prose. Fenced commands are fine; **load-bearing facts must also
  appear in prose** (so a skimmer who skips fences still learns).
- Every worked example includes **one rejected wrong reading** — the mistake a competent
  junior makes — as a paragraph, not a heading.
- Do **not** dump full man pages. Quote a flag only when the lesson needs that flag.
- Do **not** give solution bodies for student stubs. Worked examples interpret *artifacts*
  (asm shape, register story, ELF headers), not finished `TODO` implementations.
- US spelling. Expand uncommon acronyms once (PLT, ABI, ELF, PIE).

### "A bit less hand-holding" (practice side)

- `TASK.md` should not narrate every keystroke already covered in the lesson.
- Leave implementation, observation, and judgement to the student.
- Success criteria should be checkable (`ok`, exit code, or named observations) without an
  answer key file.

### Check yourself

Production prompts answerable from the lesson alone. If the only way to answer is to have
memorized an external man page, the lesson failed.

### Key takeaways

Defensible claims (3–5 bullets), not a restatement of the headings.

## README / philosophy

Lead with: learn from `LESSON.md`, prove it in `TASK.md`, use man/info as reference.
Reps still matter; untaught reps do not.