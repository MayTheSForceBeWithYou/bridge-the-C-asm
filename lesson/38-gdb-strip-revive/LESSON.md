# 38 — Breaking after `strip` removes names

`strip` deletes symbol and debug baggage the loader does not need to run the
process. The machine code remains; `nm` goes quiet; named breakpoints like
`break secret` stop resolving. You can still stop *inside* that code by planting
a breakpoint on a **raw address** you read from `objdump -d`. That judgement —
symbols are convenience, addresses are what the CPU uses — is the point of this
lab. By the end you should recover a stop with `break *0xADDR` after matching
instruction shape across stripped and unstripped dumps.

## What this lesson asks of you

Implement `secret` to return 42, confirm `./prog` works with symbols, copy and
strip a twin binary, recover the function's start address from disassembly, and
hit `break *0xADDR` under **gdb**. nvim-dap launch of the stripped file is
optional experimentation; address breakpoints are a gdb strength here.

`TASK.md` is practice. Pretty names are optional; the stop is not.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | build `./prog` with symbols / `-ggdb` | `./prog` prints `ok` |
| (manual) `cp prog prog.stripped && strip prog.stripped` | remove symbol/debug sections | stripped twin still runs |

## What disappears, what does not

After `strip prog.stripped`:

| Gone or emptied | Still present |
| --------------- | ------------- |
| Symbol table entries for `secret` / `main` | `.text` bytes for those functions |
| Much debug info | Ability to `exec` and print `ok` |
| Comfortable `break secret` / F9-by-line maps | Raw instruction addresses in `objdump -d` |

Compare `nm prog` (names) to `nm prog.stripped` (quiet / error). Compare file
sizes. Then run `./prog.stripped` — it should still behave. **Rejected wrong
reading:** "strip broke the program." Strip broke *names*, not opcodes.

## How to pick `0xADDR` from `objdump -d` (recognition)

You need a navigation move before drowning in CRT and PLT:

1. `objdump -d prog` — search for `<secret>:` (angle brackets, colon).
2. Note the **left-column instruction address** of the first insn under that
   label, and the raw bytes / mnemonics of the prologue (shape fingerprint).
3. `objdump -d prog.stripped` — there is no `<secret>:` label. Search for the
   **same byte sequence / mnemonic shape**.
4. The left-column address on that matching block is your `0xADDR`.

A typical unstripped shape (PIE addresses vary; columns do not):

```text
0000000000001180 <secret>:
    1180:	f3 0f 1e fa          	endbr64
    1184:	b8 2a 00 00 00       	mov    $0x2a,%eax
    1189:	c3                   	ret
```

| Column | Example | What it is |
| ------ | ------- | ---------- |
| Instruction address | `1180:` | Where this insn lives in the image — **this** is what `break *0x…` wants (with appropriate base / as printed for that file) |
| Raw bytes | `b8 2a 00 00 00` | Fingerprint to match after strip |
| Mnemonic | `mov $0x2a,%eax` | Human decoding (42 in hex is `0x2a`) |
| Label | `<secret>:` | Convenience that strip removes |

**Recognition rule:** the label `<secret>:` is optional metadata. The left
address plus bytes are what survive. Do not invent an address from `nm` after
strip — `nm` is empty on purpose.

### PIE / ASLR honesty

On PIE binaries the dump shows addresses as offsets in the image; gdb will still
accept `break *0xADDR` using the addresses `objdump` prints for that file when
you debug that same file consistently. If a break "misses," re-check whether you
copied an address from the *wrong* build (stripped vs not) or mixed runtime
ASLR addresses with file offsets. Sanity baseline: on unstripped `./prog`,
`break secret` still works — prove the function before you strip.

Practical session:

```text
gdb -q ./prog.stripped
(gdb) break *0xADDR
(gdb) run
(gdb) disassemble
(gdb) continue
```

## Why gdb is primary (and F9 struggles)

Named F9 breakpoints need debug info / symbols mapping line to address.
Stripped binaries starve that path. gdb's `break *0xADDR` talks in the CPU's
native language. CodeLLDB's DAP REPL *might* accept an address break — treat
success as luck, not the requirement. Contrast: with symbols still on `./prog`,
ordinary F9 on `secret` works fine — that contrast is part of the learning.

**Rejected wrong reading:** "I will F9 on the source line against the stripped
binary and call it done." Line maps want debug info; strip removed your friends.

## Worked example

**The situation.** `./prog` prints `ok`. You `cp prog prog.stripped && strip
prog.stripped`.

**Step 1 — prove symbols died.** `nm prog.stripped` errors or lists no `secret`.
Run `./prog.stripped` — still `ok`. Names ≠ opcodes.

**Step 2 — fingerprint the function.** In the unstripped `objdump -d prog`,
note the first instructions of `<secret>`. In the stripped dump, find the same
byte sequence and record its leading left-column address.

**Step 3 — break by address.** `break *0xADDR`, `run`, `disassemble`, glance at
registers (`%eax`/`%rax` should be able to show 42 for a trivial body),
`continue`. You stopped with no symbol names. That is revival of *control*, not
revival of pretty names.

**Step 4 — optional dap experiment.** Launch `./prog.stripped` under nvim-dap
and try an address break in the REPL only as curiosity — gdb remains the success
path for the TASK.


## A careful objdump navigation recipe

Busy listings drown readers in `_init`, `.plt`, and CRT. Anchor first:

1. On the **unstripped** dump, search exactly for `<secret>:` (include brackets
   and colon — same habit as searching `<main>:` in lesson 01).
2. Copy three facts onto scratch paper: left-column start address, first 8–12
   raw bytes, and the `mov`/`ret` shape if the body is tiny.
3. On the **stripped** dump, search for those raw bytes (or the distinctive
   `mov $0x2a,%eax` / equivalent immediates). Ignore missing labels.
4. Confirm you did not match a *different* function that happens to return 42
   by comparing surrounding insn count / epilogue.

**Rejected wrong reading:** "I grepped for `secret` in the stripped dump and
found nothing, so the function is gone." The *name* is gone; the bytes remain.

### When `break *0xADDR` still misses

- Wrong file: address from `prog` used against `prog.stripped` with different
  layout (rebuild one side).
- Typed `break 0xADDR` without `*` — gdb may treat it as a line number.
- ASLR confusion: mixing a runtime `%rip` from a previous live run with a
  file-offset address from `objdump` without understanding PIE. Prefer addresses
  taken from `objdump -d` on the **same** ELF you pass to gdb, then `run` under
  that gdb session.

If stuck, re-establish sanity on unstripped `./prog` with `break secret` to
prove the function still returns 42, then repeat the fingerprint path.



## What success looks like emotionally

Hitting `break *0xADDR` on a stripped binary feels less friendly than F9 on a
named line. That discomfort is the lesson landing: production artifacts are
often stripped; your ability to stop on an address keeps you in the game. You
are not restoring symbols — you are restoring *control*.



## Optional: what dap can still do with symbols on

Before stripping, F9 on `secret` in `./prog` should work like any other line
breakpoint. Do that once so you feel the *contrast* when names vanish. The
lesson is not "never use F9" — it is "know the fallback when F9's dependencies
are gone."


## Distinctions worth keeping straight

- **Stripped vs not runnable** — symbols ≠ opcodes.
- **`break secret` vs `break *0xADDR`** — name lookup versus absolute stop.
- **`nm` silence vs empty `.text`** — metadata versus code.
- **Left-column address vs raw bytes vs label** — `break *` wants the address;
  matching uses bytes; labels are optional.
- **gdb primary vs dap optional** — match the tool to address breakpoints.

## Check yourself

1. After `strip`, why can `./prog.stripped` still print `ok`?
2. How do you choose `0xADDR` if `nm` no longer lists `secret`?
3. Why is F9 on a source line unreliable for the stripped binary?
4. Which tool is the success path for this TASK's breakpoint?
5. On an `objdump` line `1184: b8 2a 00 00 00  mov $0x2a,%eax`, which token is
   the instruction address you would feed to `break *`?
6. You copied an address from `objdump -d prog` but are debugging
   `prog.stripped` and miss. What did you likely mix up?

If fuzzy, re-read the column table — then `man 1 strip` for flag spelling only.

## Key takeaways

- `strip` removes symbols/debug; it does not remove the instructions you need.
- Recover a stop by matching `objdump -d` shapes and using `break *0xADDR` in
  gdb.
- Named breakpoints and dap line maps expect symbols — stripped builds teach
  the fallback.
- Always verify the stripped binary still runs before debugging it.
- Left-column addresses and raw bytes survive; labels do not.

## Lookup (not the lesson)

- `NVIM_DAP.md` — stripped / address-break honesty
- `man 1 strip`, `man 1 nm`, `man 1 objdump`, `man 1 gdb`

Now open `TASK.md` and do the practice.
