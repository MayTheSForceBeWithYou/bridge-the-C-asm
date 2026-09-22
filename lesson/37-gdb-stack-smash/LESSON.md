# 37 — Reading a smashed return address

A small automatic buffer like `char buf[8]` sits on the stack near the saved
frame pointer and return address. If you deliberately write past its end (this
lab uses `memset(buf, 0x41, 64)` with `-fno-stack-protector`), those control
slots become `0x41` bytes. On return, the CPU pops garbage into `%rip` and you
crash or land somewhere nonsense. The educational goal is not exploit craft —
it is recognizing *corrupted control data* in a memory view and a broken
backtrace. By the end you should anchor `<leader>dm` on `buf`, read a run of
`41` bytes as overflow evidence, and treat a weird Stacks / `bt` as a symptom
of smashed control slots — not a broken debugger.

## What this lesson asks of you

Build with the Makefile's `-fno-stack-protector`, stop inside `smash`, memory-
view `buf`, continue into the failure, and compare Stacks / `bt` before versus
after corruption. Do **not** "fix" the overflow; observing it is the exercise.
Real code keeps stack protectors on for a reason — this flag is a teaching
scalpel, not a default.

`TASK.md` is practice. Treat the crash as evidence you are collecting, not as a
grader failure.

## What `make` is naming for you

| You type | Rough equivalent | Note |
| -------- | ---------------- | ---- |
| `make` | compile/link with `-fno-stack-protector` and `-ggdb` | Required for the overflow to reach control data as written |
| `make run` | run `./prog` | Crash / weird exit is **expected success** |

Do not "improve" the Makefile by turning protectors back on for this exercise —
that hides the phenomenon the lab is about.

## Where `buf` sits relative to control data

Exact layout depends on prologue and compiler version, but the story is stable:
higher addresses toward the caller's frame hold saved `%rbp` (if used) and the
return address pushed by `call`. `buf` at a lower address grows toward those
slots when you write forward with rising indices. Sixty-four `0x41` bytes from
an 8-byte array walk well past the buffer into whatever the compiler placed
above it.

### Memory-view anchors (recognition)

1. F9 inside `smash` (before or on the `memset` line).
2. Launch existing → `./prog`.
3. From Scopes, find `buf` — note its **address** (`&buf` / pointer value).
4. `<leader>dm` → paste **that address** (not `%rip`, not a random register).
5. Read bytes: after the wide write you should see a long run of `41` (ASCII
   `'A'`). Count mentally: the array only owns 8 bytes; the rest is neighboring
   stack.

gdb twins: `x/16xb buf`, `x/64xb buf`. Same recognition rule — base address is
`buf`, pattern is `0x41`.

**Rejected wrong reading:** "I do not see the letter A in Scopes so nothing
happened." Scopes may still pretty-print `buf` as a short string or array slice;
the overflow lives in *adjacent* bytes. Memory view / `x/` is how you see past
the declared object.

## Why Stacks / `bt` go weird

A healthy backtrace walks saved frame pointers / unwind info to name callers.
Once the return address (and maybe saved `%rbp`) are `0x41414141…`, unwind
looks like fiction: frames missing, addresses outside your text, or a sudden
stop. That nonsense *is* the lesson.

| Before smash | After smash / on fault |
| ------------ | ---------------------- |
| Stacks / `bt` show `main` → `smash` (sensible) | Frames missing, `0x41414141`-ish PCs, or abrupt stop |
| `&buf` points at a small object | Memory at `&buf` shows long `41` run |
| Return slot holds a code address | Return slot holds `0x4141414141414141` pattern |

**Rejected wrong reading:** "the debugger is broken." Prefer: "control data on
the stack is no longer a valid code address."

Continue (`<leader>dc` / F5, or gdb `continue`) after inspecting `buf` and let
the function return. Crash or wild jump confirms the corruption took effect.
Lesson 39 will map a *clean* crash address with `addr2line`; here the PC after
smash may be nonsense — that contrast is useful later.

## Keys and gdb commands for this lab

| Tooling | Action |
| ------- | ------ |
| F9 | Break inside `smash` |
| F5 / `<leader>dc` | Launch existing → `./prog`; later continue into doom |
| `<leader>dm` | Memory-view `&buf` |
| Stacks pane | Photograph unwind before/after |
| `<leader>dt` | Terminate when you have enough evidence |
| gdb `x/64xb buf`, `bt`, `disassemble smash` | CLI twins |

## Worked example

**The situation.** You are stopped on the first line of `smash` before or just
as `memset` runs.

**Step 1 — photograph clean stack intent.** Note `&buf`. If you can still see a
sane return address at the expected slot before the wide write (dump upward
from `&buf` carefully, or examine `$rbp+8` style slots if you know the frame),
glance at it. The rejected wrong reading is: "I will only look after it
crashes." Comparing before/after teaches more than staring at a core alone.

**Step 2 — run the overflow.** Step over / continue through `memset`. Memory
view shows `0x41` spanning far past 8 bytes. Count: you asked for 64 bytes; the
array only owns 8 — the rest is neighboring stack.

**Step 3 — continue to doom.** Stacks panel or `bt` after the fault. Record that
frames look wrong. You are done when you can explain *which* kind of data was
overwritten (buffer versus return address), not when you have a working patch.


## Relating this smash to lessons 35 and 39

Lesson 35 taught you to read `[$rsp]` as a return address after `call`. This
lab *destroys* that slot with `0x41` bytes. The memory-view skill is the same
(`<leader>dm` / `x/`); the story is inverted: instead of photographing a healthy
return address, you photograph a ruined one.

Lesson 39 maps a clean faulting PC with `addr2line`. After a smash, `%rip` may
become `0x41414141…` — not a location in your `.text`. Keep that contrast:
tooling that needs a real code address will not save a smashed return.

### What `0x41` means visually

`memset(..., 0x41, 64)` paints ASCII `'A'`. In hex dumps you see `41 41 41 41`.
In a 64-bit slot that overwrote a return address you may read
`0x4141414141414141` in `x/1xg`. Both representations are the same vandalism.

**Rejected wrong reading:** "I need to find the string `AAAA` in Scopes." You
need the byte pattern in memory view / `x/`, including past the eight bytes the
type system thinks `buf` owns.

### Safety note (non-negotiable)

Stack protectors (`-fstack-protector*`) exist to make this class of bug louder
and harder to turn into control-flow hijacks. This exercise disables them so you
can *see* the corruption. Shipping code without protectors because "the lab did
it" is malpractice. When the TASK says do not fix the overflow, it means do not
"solve" the crash for a green `ok` — not that overflows are desirable.



## What not to optimize while "observing"

Students sometimes shorten `memset` to 8 bytes "just to see the buffer," or
re-enable stack protectors to stop the crash, then claim the lab is done
because Scopes looked fine. That edits away the phenomenon. Keep the wide
write, keep `-fno-stack-protector`, take the memory-view photo, then continue
into the broken unwind. Observation requires leaving the bug intact.


## Distinctions worth keeping straight

- **Payload bytes vs control slots** — `buf` contents versus saved `%rip`.
- **`-fno-stack-protector` lab vs production** — canary disabled on purpose.
- **Crash as success vs crash as mystery** — here the crash is evidence.
- **Stacks panel vs healthy `bt`** — both report unwind; smashed frames lie.
- **Pretty `buf` in Scopes vs adjacent bytes in memory view** — declared object
  versus overrun.

## Check yourself

1. Why does writing 64 bytes into `buf[8]` threaten the return address?
2. What Makefile flag deliberately disables the usual mitigation here?
3. After the smash, why might dap Stacks disagree with a clean mental model of
   `main → smash`?
4. Is the right follow-up to "fix" `memset` length for this TASK?
5. What exact address do you paste into `<leader>dm` to see the `0x41` run?
6. Why might lesson 39's `addr2line` be less helpful on the PC you get *after*
   this smash than on a clean NULL deref?

If fuzzy, re-read the before/after table — do not start reading exploit writeups.

## Key takeaways

- Stack buffers sit near saved control data; overrun can replace the return
  address with payload bytes (here `0x41`).
- Memory view / `x/` show the pattern; Stacks / `bt` show unwind failure after.
- Anchor `<leader>dm` on `&buf`; do not trust a short Scopes pretty-print alone.
- This lab disables stack protectors on purpose — never copy the pattern into
  real programs.
- Observing intentional UB is the success criterion, not a green `ok`.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `<leader>dm`, Stacks, continue / terminate
- `man 1 gdb` — `help x`, `help bt`
- `man 1 gcc` — `-fstack-protector` / `-fno-stack-protector`

Now open `TASK.md` and do the practice.
