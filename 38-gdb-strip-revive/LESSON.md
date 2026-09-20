# 38 — Breaking after `strip` removes names

`strip` deletes symbol and debug baggage the loader does not need to run the
process. The machine code remains; `nm` goes quiet; named breakpoints like
`break secret` stop resolving. You can still stop *inside* that code by planting
a breakpoint on a **raw address** you read from `objdump -d`. That judgement —
symbols are convenience, addresses are what the CPU uses — is the point of this
lab.

## What this lesson asks of you

Implement `secret` to return 42, confirm `./prog` works with symbols, copy and
strip a twin binary, recover the function's start address from disassembly, and
hit `break *0xADDR` under **gdb**. nvim-dap launch of the stripped file is
optional experimentation; address breakpoints are a gdb strength here.

## What disappears, what does not

After `strip prog.stripped`:

- Symbol table entries for `secret` / `main` are gone or emptied — `nm` fails or
  shows nothing useful.
- The `.text` bytes for those functions are still there.
- File size usually drops; the program still prints `ok` when run *without* a
  debugger needing names.

Compare `objdump -d prog` (with names) to `objdump -d prog.stripped`. The
instruction shape at the former `secret` label is your fingerprint: same
prologue bytes at a listed address. On PIE binaries the dump shows addresses
as offsets in the image; gdb will still accept `break *0xADDR` using the
addresses `objdump` prints for that file when ASLR is disabled for the session
or when you use the unrelocated dump consistently — if a break "misses," re-check
whether you copied an address from the wrong build or need `break *secret`
before strip as a sanity baseline on `./prog`.

Practical workflow that stays honest: find the address in `objdump -d
prog.stripped` for the block that matches `secret`'s instructions from the
unstripped dump, then `gdb -q ./prog.stripped` → `break *0xADDR` → `run`.

## Why gdb is primary

Named F9 breakpoints need debug info / symbols mapping line to address.
Stripped binaries starve that path. gdb's `break *0xADDR` talks in the CPU's
native language. CodeLLDB's DAP REPL *might* accept an address break — treat
success as luck, not the requirement. Contrast: with symbols still on `./prog`,
ordinary F9 on `secret` works fine — that contrast is part of the learning.

## Worked example

**The situation.** `./prog` prints `ok`. You `cp prog prog.stripped && strip
prog.stripped`.

**Step 1 — prove symbols died.** `nm prog.stripped` errors or lists no `secret`.
The rejected wrong reading is: "strip broke the program." Run `./prog.stripped`
— it should still behave.

**Step 2 — fingerprint the function.** In the unstripped `objdump -d prog`,
note the first instructions of `<secret>`. In the stripped dump, find the same
byte sequence and record its leading address.

**Step 3 — break by address.** `break *0xADDR`, `run`, `disassemble`, glance at
registers, `continue`. You stopped with no symbol names. That is revival of
control, not revival of pretty names.

## Distinctions worth keeping straight

- **Stripped vs not runnable** — symbols ≠ opcodes.
- **`break secret` vs `break *0xADDR`** — name lookup versus absolute stop.
- **`nm` silence vs empty `.text`** — metadata versus code.
- **gdb primary vs dap optional** — match the tool to address breakpoints.

## Check yourself

1. After `strip`, why can `./prog.stripped` still print `ok`?
2. How do you choose `0xADDR` if `nm` no longer lists `secret`?
3. Why is F9 on a source line unreliable for the stripped binary?
4. Which tool is the success path for this TASK's breakpoint?

## Key takeaways

- `strip` removes symbols/debug; it does not remove the instructions you need.
- Recover a stop by matching `objdump -d` shapes and using `break *0xADDR` in
  gdb.
- Named breakpoints and dap line maps expect symbols — stripped builds teach
  the fallback.
- Always verify the stripped binary still runs before debugging it.

## Lookup (not the lesson)

- `NVIM_DAP.md` — stripped / address-break honesty
- `man 1 strip`, `man 1 nm`, `man 1 objdump`, `man 1 gdb`

Now open `TASK.md` and do the practice.
