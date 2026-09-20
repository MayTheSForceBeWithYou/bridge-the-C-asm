# 35 — `%rsp` after `call` and each `push`
<!-- concepts: c.p2.stack.frame, c.p5.gdb.stack -->

A `call` does two things you can see with a debugger: it pushes an 8-byte return
address, then transfers control. Each later `push` of a callee-saved register
drops `%rsp` by another 8 on this ABI. If you only step by source line, those
updates blur together. This lesson trains the judgement of reading `$rsp` and
the memory under it so "the stack" stops being a cartoon and becomes eight-byte
slots you can dump.

## What this lesson asks of you

Implement a tiny `scratch` that pushes three registers, pops them, and returns
42. Stop at the harness call site, enter `scratch`, and explain what sits at
`$rsp` — first the return address, then the saved registers as you push.
nvim-dap is excellent for call-site → step-into and for `<leader>dm` on the
stack pointer. **Instruction-precise** watching of each `push` belongs to gdb
`stepi`, because F10/F11 are not `stepi`.

`TASK.md` is practice. Keep the honest tool split; do not pretend the editor
keys single-step machine code.

## What `call` leaves on the stack

Before `call`, `%rsp` points at whatever the caller considers top-of-stack
(alignment rules matter in the wider ABI story; here you care about the delta).
Executing `call` writes the address of the following instruction to `[rsp-8]`
and sets `%rsp` to that new top. When you first stop *inside* `scratch`,
`x/1xg $rsp` (or memory-view the SP address) should show a code address back in
the harness / `call_site` — that is your ticket home at `ret`.

The rejected wrong reading is: "`%rbp` is the return address." On the common
`-O0` frame setup, saved `%rbp` is a *later* slot after a `push %rbp`; the
immediate `[rsp]` at true entry (before a prologue) is the return address. Your
`scratch` stub may skip a frame pointer entirely — then every push is just
another 8-byte slot descending from that return address.

## Why F11 is not enough for "each push"

nvim-dap **F10 / F11 / F12** usually step by **source line** or statement under
CodeLLDB. A label like `scratch:` with three `push` instructions may be one
"line" of `.s` or poorly mapped, so one F11 can execute more than one push.
When the TASK asks how many bytes `%rsp` drops **per push**, use:

```text
(gdb) stepi
(gdb) info registers rsp
(gdb) x/8xg $rsp
```

after entering the callee. Expect `%rsp` to decrease by `0x8` each push.
nvim-dap still earns its keep: break on `call_site`, F11 into `scratch`, note
SP in Scopes, `<leader>dm` at that address to see the return slot and later
pushed values as you continue.

## Worked example

**The situation.** `call_site` calls `scratch`; you break on `call_site`, run,
and `stepi` once (gdb) to execute the `call`.

**Step 1 — right after `call`.** `%rsp` has dropped by 8 from the pre-call
value. `x/8xg $rsp` shows the return address in the first slot. The rejected
wrong reading is: "the first slot is the first register I plan to push." You
have not pushed yet; you only entered.

**Step 2 — break at `scratch` / step pushes.** Each `push` stores a register
and decrements `%rsp` by 8. After three pushes, the return address sits at
`[rsp+24]` if nothing else intervened — count carefully from your dump. The
rejected wrong reading is: "pushes overwrite the return address." They write
*below* it (lower addresses); `ret` still finds the return address once you
`pop` back to the entry SP.

**Step 3 — pops and `ret`.** Pops raise `%rsp`. Final `ret` pops the return
address into `%rip`. `%rax` should be 42 for the harness.

## Distinctions worth keeping straight

- **Return address slot vs saved registers** — first gift of `call` versus your
  pushes.
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

## Key takeaways

- `call` pushes an 8-byte return address; each `push` drops `%rsp` by another 8.
- Dump `$rsp` (`x/8xg` or `<leader>dm`) to *see* slots, do not only imagine them.
- Use gdb `stepi` when the question is per-instruction; F11 is source-level.
- Pops restore `%rsp` so `ret` still finds the original return address.

## Lookup (not the lesson)

- `NVIM_DAP.md` — F11 ≠ `stepi`; `<leader>dm`; call-site workflow
- `man 1 gdb` — `help stepi`, `help x`
- `man 1 as` — `push` / `pop` / `ret` as you implement `scratch`

Now open `TASK.md` and do the practice.
