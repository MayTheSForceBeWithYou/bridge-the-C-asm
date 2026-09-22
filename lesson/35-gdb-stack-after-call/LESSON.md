# 35 — `%rsp` after `call` and each `push`

A `call` does two things you can see with a debugger: it pushes an 8-byte return
address, then transfers control. Each later `push` of a callee-saved register
drops `%rsp` by another 8 on this ABI. If you only step by source line, those
updates blur together. This lesson trains reading `$rsp` and the memory under it
so "the stack" becomes eight-byte slots you can dump. It also hardens the most
expensive mix-up in this Track C block: **F11 ≠ `stepi`**.

## What this lesson asks of you

Implement a tiny `scratch` that pushes three registers, pops them, and returns
42. Stop at the harness call site, enter `scratch`, and explain what sits at
`$rsp` — first the return address, then the saved registers as you push.
nvim-dap is excellent for call-site → step-into and for `<leader>dm` on the
stack pointer. **Instruction-precise** watching of each `push` belongs to gdb
`stepi`, because F10/F11 are not `stepi`.

`TASK.md` is practice. Keep the honest tool split; do not pretend the editor
keys single-step machine code.

## What `make` is naming for you

| You type | Rough equivalent | File afterward |
| -------- | ---------------- | -------------- |
| `make` | assemble `scratch.s`, link harness with `-ggdb` | `./stack_test` |
| `make run` | run harness | prints `ok` when `scratch` returns 42 |

## What `call` leaves on the stack

Before `call`, `%rsp` points at whatever the caller considers top-of-stack
(alignment rules matter in the wider ABI story; here you care about the delta).
Executing `call` writes the address of the following instruction to `[rsp-8]`
and sets `%rsp` to that new top. When you first stop *inside* `scratch`,
`x/1xg $rsp` (or memory-view the SP address) should show a code address back in
the harness / `call_site` — that is your ticket home at `ret`.

### How to read an `x/8xg $rsp` dump

A typical gdb examine after entry looks like (addresses and values vary with
PIE/ASLR; column *meaning* does not):

```text
0x7fffffffe1a0:  0x00005555555551a2  0x0000000000000000
0x7fffffffe1b0:  …
```

| Field | Example | What it is |
| ----- | ------- | ---------- |
| Left address | `0x7fffffffe1a0:` | Where this dump line starts (often current `$rsp`) |
| First 8-byte slot | `0x00005555555551a2` | At true entry: usually the **return address** (code address in the harness) |
| Later slots | pushed register values | Appear as you `push` — at lower addresses as `%rsp` drops |

**Recognition rule — three hex numbers people conflate:**

1. **Value of `%rsp`** — a pointer (data address). Paste *this* into `<leader>dm`
   / use as `$rsp` for `x/`.
2. **Bytes at `[$rsp]` right after `call`** — the return address (a *code*
   address). Looks pointer-shaped but means "where `ret` will jump."
3. **A pushed register's value** — whatever was in `%rbx` / `%r12` / … — often
   not a code pointer.

Do not call every hex blob on the stack "the return address."

**Rejected wrong reading:** "`%rbp` is the return address." On the common `-O0`
frame setup, saved `%rbp` is a *later* slot after a `push %rbp`; the immediate
`[rsp]` at true entry (before a prologue) is the return address. Your `scratch`
stub may skip a frame pointer entirely — then every push is just another 8-byte
slot descending from that return address.

## Why F11 is not enough for "each push"

nvim-dap **F10 / F11 / F12** usually step by **source line** or statement under
CodeLLDB. Rough gdb equivalents: `next` / `step` / `finish` — **not** `stepi` /
`nexti`. A label like `scratch:` with three `push` instructions may be one
"line" of `.s` or poorly mapped, so one F11 can execute more than one push.

When the TASK asks how many bytes `%rsp` drops **per push**, use:

```text
(gdb) break call_site
(gdb) run
(gdb) stepi              # execute the call
(gdb) info registers rsp
(gdb) x/8xg $rsp
(gdb) stepi              # each push
(gdb) info registers rsp
```

Expect `%rsp` to decrease by `0x8` each push.

### What nvim-dap is still for

| Key | Role |
| --- | ---- |
| F9 | Break on `call_site` in the harness |
| F5 / `<leader>dc` | Launch existing → `./stack_test` |
| F11 | Enter `scratch` (source grain — good enough to arrive) |
| `<leader>dm` | Paste **value of `%rsp`** to see slots |
| Scopes → registers | Read `%rsp` / `%rax` without leaving the editor |
| `<leader>dt` | Terminate |

Use dap to *arrive* and to *photograph*; use gdb `stepi` to *measure* per-push
deltas. **Rejected wrong reading:** "I F11'd three times, so I watched three
pushes." You watched up to three *source steps*; the machine may have done more
or less.

## Memory-view anchors for the stack

1. At the stop inside `scratch`, read `%rsp` from Scopes (register group) or
   `info registers rsp`.
2. Copy that **pointer value** (the SP itself).
3. `<leader>dm` → paste it. Prefer an 8-byte / hex view if the UI offers widths.
4. First slot at entry ≈ return address; after pushes, count 8-byte strides
   toward lower addresses.

If you accidentally paste the return-address *contents* as the viewer base, you
are looking at code bytes in `.text`, not the stack — another recognition
failure. Anchor on SP.

## Worked example

**The situation.** `call_site` calls `scratch`; you break on `call_site`, run,
and `stepi` once (gdb) to execute the `call`.

**Step 1 — right after `call`.** `%rsp` has dropped by 8 from the pre-call
value. `x/8xg $rsp` shows the return address in the first slot. The rejected
wrong reading is: "the first slot is the first register I plan to push." You
have not pushed yet; you only entered.

**Step 2 — step pushes with `stepi`.** Each `push` stores a register and
decrements `%rsp` by 8. After three pushes, the return address sits at
`[rsp+24]` if nothing else intervened — count carefully from your dump. The
rejected wrong reading is: "pushes overwrite the return address." They write
*below* it (lower addresses); `ret` still finds the return address once you
`pop` back to the entry SP.

**Step 3 — pops and `ret`.** Pops raise `%rsp`. Final `ret` pops the return
address into `%rip`. `%rax` should be 42 for the harness.

**Step 4 — dap photograph.** Same arrival with F11 into `scratch`, `<leader>dm`
on SP — confirm you can *see* the return slot even when you use gdb for the
per-insn cadence.


## Counting slots after three pushes (worked arithmetic)

Suppose at entry `$rsp = R` and `[R]` holds the return address. After one
`push`, `$rsp = R-8`, and the pushed value sits at `[R-8]`. After three pushes,
`$rsp = R-24`. The return address remains at address `R`, which is now
`[$rsp+24]`.

Write that once with *your* numbers from `info registers rsp` and `x/8xg`.
The algebra is the recognition rule; memorizing "plus 24" without measuring SP
is fragile if you pushed a different count.

**Rejected wrong reading:** "The return address moved to a new location when I
pushed." The return address bytes stay put; `%rsp` moves away from them toward
lower addresses.



## Launch reminder and binary name

`make` produces `./stack_test` (NAME from the Makefile). Launch existing must
target that path. Building only `scratch.s` or only the harness recreates the
lesson-33/71 failure mode. Arrive with dap or gdb; measure with `stepi`.


## Distinctions worth keeping straight

- **Return address slot vs saved registers** — first gift of `call` versus your
  pushes.
- **Value of `%rsp` vs bytes at `$rsp`** — pointer versus pointee.
- **F11 vs `stepi`** — enter the function versus walk each insn.
- **dap memory view vs `x/8xg`** — same bytes; gdb wins for per-insn cadence.
- **`%rsp` decreasing vs "stack growing"** — on x86-64 the stack grows toward
  lower addresses, so "grow" means smaller `%rsp`.

## Check yourself

1. Immediately after `call` enters `scratch`, what is at `[$rsp]`?
2. How many bytes does `%rsp` change for one `push` of a 64-bit register?
3. Why might F11 fail to show three distinct `%rsp` updates for three pushes?
4. After three pushes and no other stack ops, where is the return address
   relative to the current `%rsp`?
5. You open `<leader>dm` — do you paste the return-address *value* from the first
   slot, or the value of `%rsp` itself, to start the dump at the top of stack?
6. Is `%rbp` at entry automatically the return address?

If fuzzy, re-read the dump table — then `help stepi` / `help x` for spelling.

## Key takeaways

- `call` pushes an 8-byte return address; each `push` drops `%rsp` by another 8.
- Dump `$rsp` (`x/8xg` or `<leader>dm`) to *see* slots; decode SP value vs slot
  contents vs code addresses.
- Use gdb `stepi` when the question is per-instruction; F11 is source-level.
- Pops restore `%rsp` so `ret` still finds the original return address.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F11 ≠ `stepi`; `<leader>dm`; call-site workflow
- `man 1 gdb` — `help stepi`, `help x`
- `man 1 as` — `push` / `pop` / `ret` as you implement `scratch`

Now open `TASK.md` and do the practice.
