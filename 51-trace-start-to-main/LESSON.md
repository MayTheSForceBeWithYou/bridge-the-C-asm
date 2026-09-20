# 51 — From `_start` to `main`
<!-- concepts: c.p5.elf.crt, c.p5.asm.start -->

Your C program does not begin at `main`. The ELF entry is `_start` (often from
`crt1.o`), which eventually calls into libc startup — commonly `__libc_start_main` —
which then calls your `main` with `argc`, `argv`, and `envp`. This lesson teaches you
to recover that path from disassembly when no man page for `__libc_start_main` exists
on this machine.

## What this lesson asks of you

Disassemble the linked program, find `<_start>`, and write a hand trace of how control
reaches `main`, citing instruction lines you saw. Optional: gdb `break _start` and
`stepi` until `main`. Use `info libc` if helpful — not a fake man page.

## What startup is doing

Roughly:

1. Kernel jumps to the ELF entry (`_start`).
2. Startup aligns the stack, gathers argc/argv/envp from the initial image.
3. It calls a libc helper (`__libc_start_main` or similar) with function pointers
   including `&main`.
4. That helper finishes runtime init and calls `main`.
5. After `main` returns, exit processing runs.

Exact symbol names vary slightly with libc/version; trust *your* `objdump` output.

## How to look

```
objdump -d ./prog | grep -A40 '<_start>:'
```

Follow `call` targets. You may also see `.init` / constructors later; for this exercise
the spine to `main` is enough.

## Worked example

**The situation.** Your notes say: `_start` calls `__libc_start_main` with `main`'s
address in a register/argument slot; `__libc_start_main` later calls `main`.

**Step 1 — find `_start`.** It is short. The rejected wrong reading is: "the first
`call` in `_start` is already `main`." Usually an intermediate libc startup call sits
in between.

**Step 2 — identify the helper.** Match the PLT or direct symbol name in the dump.
There is no `man 3 __libc_start_main` here — reason from disassembly + `info libc`.
The rejected wrong reading is: "without a man page I cannot know anything." The bytes
you disassembled *are* the documentation for this binary.

**Step 3 — connect argc/argv.** Startup passes the stack-derived argc/argv into `main`
per the C ABI (`%rdi`, `%rsi`, …). Optional stepi confirms.

## Distinctions worth keeping straight

- **ELF entry vs `main`** — entry is `_start`; `main` is a callback from libc.
- **Missing man page vs missing mechanism** — inspect the binary.
- **PLT call to startup vs your PLT to printf** — same PLT machinery, different symbols.
- **Freestanding `_start` (Track D) vs hosted `_start`** — hosted chains into libc;
  freestanding *is* your whole program.

## Check yourself

1. What symbol does the ELF header use as the entry point for a normal gcc binary?
2. Why might you not see a direct `call main` inside `_start`?
3. Where should you look if `__libc_start_main` has no local man page?
4. How do argc/argv get to `main` in the hosted world?

## Key takeaways

- Hosted programs reach `main` through CRT/libc startup from `_start`.
- Disassembly is authoritative when manuals are missing.
- `__libc_start_main` (or equivalent) is the usual bridge.
- Track D freestanding work intentionally skips this chain.

## Lookup (not the lesson)

- `man 1 objdump`, `info libc`, `man 5 elf`, `man 1 gdb`

Now open `TASK.md` and do the practice.
