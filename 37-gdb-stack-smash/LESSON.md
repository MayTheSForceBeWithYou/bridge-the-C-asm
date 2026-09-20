# 37 — Reading a smashed return address
<!-- concepts: c.p2.arrays.bounds, c.p2.overflow.adjacent, c.p2.stack.frame -->

A small automatic buffer like `char buf[8]` sits on the stack near the saved
frame pointer and return address. If you deliberately write past its end (this
lab uses `memset(buf, 0x41, 64)` with `-fno-stack-protector`), those control
slots become `0x41` bytes. On return, the CPU pops garbage into `%rip` and you
crash or land somewhere nonsense. The educational goal is not exploit craft —
it is recognizing *corrupted control data* in a memory view and a broken
backtrace.

## What this lesson asks of you

Build with the Makefile's `-fno-stack-protector`, stop inside `smash`, memory-
view `buf`, continue into the failure, and compare Stacks / `bt` before versus
after corruption. Do **not** "fix" the overflow; observing it is the exercise.
Real code keeps stack protectors on for a reason — this flag is a teaching
scalpel, not a default.

`TASK.md` is practice. Treat the crash as evidence you are collecting, not as a
grader failure.

## Where `buf` sits relative to control data

Exact layout depends on prologue and compiler version, but the story is stable:
higher addresses toward the caller's frame hold saved `%rbp` (if used) and the
return address pushed by `call`. `buf` at a lower address grows toward those
slots when you write forward with rising indices. Sixty-four `0x41` bytes from
an 8-byte array walk well past the buffer into whatever the compiler placed
above it.

Under nvim-dap: F9 inside `smash`, launch `./prog`, `<leader>dm` on `buf`'s
address from Scopes. You should see a run of `41` bytes. Under gdb: `x/16xb
buf` or wider (`x/64xb buf`). Optionally `disassemble smash` and relate offsets
to `%rbp` — the same spatial habit lesson 35 used on `$rsp` after `call`.

## Why Stacks / `bt` go weird

A healthy backtrace walks saved frame pointers / unwind info to name callers.
Once the return address (and maybe saved `%rbp`) are `0x41414141…`, unwind
looks like fiction: frames missing, addresses outside your text, or a sudden
stop. That nonsense *is* the lesson. The rejected wrong reading is: "the
debugger is broken." Prefer: "control data on the stack is no longer a valid
code address."

Continue (`<leader>dc` / F5, or gdb `continue`) after inspecting `buf` and let
the function return. Crash or wild jump confirms the corruption took effect.
Lesson 39 will map a *clean* crash address with `addr2line`; here the PC after
smash may be nonsense — that contrast is useful later.

## Worked example

**The situation.** You are stopped on the first line of `smash` before or just
as `memset` runs.

**Step 1 — photograph clean stack intent.** Note `&buf`. If you can still see
a sane return address at the expected slot before the wide write, glance at it.
The rejected wrong reading is: "I will only look after it crashes." Comparing
before/after teaches more than staring at a core alone.

**Step 2 — run the overflow.** Step over / continue through `memset`. Memory
view shows `0x41` spanning far past 8 bytes. Count: you asked for 64 bytes; the
array only owns 8 — the rest is neighboring stack.

**Step 3 — continue to doom.** Stacks panel or `bt` after the fault. Record that
frames look wrong. You are done when you can explain *which* kind of data was
overwritten (buffer versus return address), not when you have a working patch.

## Distinctions worth keeping straight

- **Payload bytes vs control slots** — `buf` contents versus saved `%rip`.
- **`-fno-stack-protector` lab vs production** — canary disabled on purpose.
- **Crash as success vs crash as mystery** — here the crash is evidence.
- **Stacks panel vs healthy `bt`** — both report unwind; smashed frames lie.

## Check yourself

1. Why does writing 64 bytes into `buf[8]` threaten the return address?
2. What Makefile flag deliberately disables the usual mitigation here?
3. After the smash, why might dap Stacks disagree with a clean mental model of
   `main → smash`?
4. Is the right follow-up to "fix" `memset` length for this TASK?

## Key takeaways

- Stack buffers sit near saved control data; overrun can replace the return
  address with payload bytes (here `0x41`).
- Memory view / `x/` show the pattern; Stacks / `bt` show unwind failure after.
- This lab disables stack protectors on purpose — never copy the pattern into
  real programs.
- Observing intentional UB is the success criterion, not a green `ok`.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `<leader>dm`, Stacks, continue / terminate
- `man 1 gdb` — `help x`, `help bt`
- `man 1 gcc` — `-fstack-protector` / `-fno-stack-protector`

Now open `TASK.md` and do the practice.
