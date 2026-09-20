# 70 — Bytes behind a struct and array
<!-- concepts: c.p2.memory.variable, c.p5.dap.memory -->

Scopes will happily show `pt.x == 0x11`. Memory view shows *why*: little-endian
`11 00 00 00` at `&pt`, then `y`, then the ASCII bytes of `tag`. This lesson
ties Track A layout intuition to **`<leader>dm`** (and optionally gdb `x/`) so
you trust bytes when pretty-printers lie or when you are staring at a raw
buffer after a smash or a protocol decode.

## What this lesson asks of you

Implement `fill_demo` so `Point` and `arr` match the harness, break at a moment
when those objects are live, open `<leader>dm` on their addresses, and find
`0x11`, `0x22`, `ABC`, and the four ints `10,20,30,40` as raw memory. Correlate
with `x/16xb &pt` / `x/4wd arr` if you like the CLI spelling.

`TASK.md` is practice. The teaching goal is pattern recognition in hex, not
memorizing dap panel names.

## What you should see in memory

For `struct Point { int x; int y; char tag[4]; }` on typical x86-64 SysV layout
with no packing surprises: four bytes of `x`, four of `y`, four of `tag`
(including the NUL). Little-endian means `0x11` appears as `11` in the first
byte of `x`, not as a big-endian word-shaped blob. ASCII `'A','B','C','\0'` are
`41 42 43 00`. The `int arr[4]` filled with `(i+1)*10` is four 32-bit
little-endian words: `0a 00 00 00`, `14 00 00 00`, `1e 00 00 00`, `28 00 00 00`.

Break at the start of `fill_demo` if you want to watch fills appear as you
step, or just after `fill_demo` returns in `main` if you only care about final
layout — after-return is often easier for a stable photograph because locals in
`main` stay live for Scopes.

## `<leader>dm` workflow

1. Stop with Scopes showing `pt` / `arr` / `p`.
2. Copy an address (`&pt`, `p`, or `arr` — pointer value, not the string
   `"pt"`).
3. `<leader>dm` — paste into the float memory viewer (or use the bottom memory
   panel).
4. Add a Watch on `pt.x` or `arr[2]` if you want the pretty value beside the
   bytes.

gdb twin commands: `x/16xb &pt`, `x/4wd arr`, `p pt`. Same bytes, different
keyboard. Lesson 35 used this skill on `$rsp`; here the target is a struct you
filled yourself, which makes the expected pattern knowable in advance.

## Worked example

**The situation.** You stopped after `fill_demo` filled `pt` and `arr`.

**Step 1 — struct bytes.** Memory at `&pt`: find `11 00 00 00`, then `22 00 00
00`, then `41 42 43 00`. The rejected wrong reading is: "I do not see the
integer eleven as ASCII characters `11`." You are hunting hex byte `11`, not
digit characters.

**Step 2 — array words.** Memory at `arr`: four little-endian ints 10..40. The
rejected wrong reading is: "array elements are adjacent one-byte fields." They
are 4-byte `int`s unless you declared a different type.

**Step 3 — cross-check.** Watch `pt.x` or `p pt` matches the first word you
saw. When pretty and raw disagree, raw wins for layout bugs; pretty wins for
speed once you trust layout. If `tag` shows `ABC` in Scopes but memory lacks
`43`, you are looking at the wrong address or an unfilled buffer.

## Distinctions worth keeping straight

- **Pretty local vs raw bytes** — Scopes versus `<leader>dm` / `x/`.
- **Little-endian int vs ASCII digit characters** — `0x11` versus `'1','1'`.
- **Break inside fill vs after return** — watching paint versus final photo.
- **Struct field order vs memory order** — source order matches layout here;
  packing labs elsewhere will break that comfort.

## Check yourself

1. In little-endian memory, which byte of `pt.x` holds `0x11`?
2. What key opens the dap memory float?
3. How wide is each `arr[i]` in bytes for `int arr[4]`?
4. Why might after-return be easier than breaking on the first line of
   `fill_demo`?

## Key takeaways

- `<leader>dm` turns addresses from Scopes into readable bytes.
- Little-endian ints and ASCII tags are recognizable patterns once you know
  what to hunt.
- gdb `x/` is the same skill with different spelling.
- Layout bugs show up in raw memory even when a single field print looks fine.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `<leader>dm`, Watches, memory panel
- `man 1 gdb` — `help x`

Now open `TASK.md` and do the practice.
