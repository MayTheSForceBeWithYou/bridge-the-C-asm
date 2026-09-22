# 70 — Bytes behind a struct and array

Scopes will happily show `pt.x == 0x11`. Memory view shows *why*: little-endian
`11 00 00 00` at `&pt`, then `y`, then the ASCII bytes of `tag`. This lesson
ties Track A layout intuition to **`<leader>dm`** (and optionally gdb `x/`) so
you trust bytes when pretty-printers lie or when you are staring at a raw
buffer after a smash or a protocol decode. By the end you should anchor the
viewer on the right address and recognize LE ints versus ASCII without guessing.

## What this lesson asks of you

Implement `fill_demo` so `Point` and `arr` match the harness, break at a moment
when those objects are live, open `<leader>dm` on their addresses, and find
`0x11`, `0x22`, `ABC`, and the four ints `10,20,30,40` as raw memory. Correlate
with `x/16xb &pt` / `x/4wd arr` if you like the CLI spelling.

`TASK.md` is practice. The teaching goal is pattern recognition in hex, not
memorizing dap panel names.

## What `make` is naming for you

| You type | Rough equivalent | Afterward |
| -------- | ---------------- | --------- |
| `make` | build `./prog` with `-ggdb` | objects ready to inspect |
| `make run` | run harness | `ok` when layout matches |

## What you should see in memory

For `struct Point { int x; int y; char tag[4]; }` on typical x86-64 SysV layout
with no packing surprises:

| Field | Size | Expected bytes (LE / ASCII) |
| ----- | ---- | --------------------------- |
| `x` | 4 | `11 00 00 00` for `0x11` |
| `y` | 4 | `22 00 00 00` for `0x22` |
| `tag` | 4 | `41 42 43 00` for `"ABC"` |

Little-endian means `0x11` appears as `11` in the **first** byte of `x`, not as
a big-endian word-shaped blob and not as ASCII characters `'1','1'`.

The `int arr[4]` filled with `(i+1)*10` is four 32-bit little-endian words:

```text
0a 00 00 00  14 00 00 00  1e 00 00 00  28 00 00 00
```

(that is 10, 20, 30, 40).

### Pretty vs raw (recognition)

| View | Shows | Failure mode |
| ---- | ----- | ------------ |
| Scopes / `<leader>de` | `pt.x == 17` or `0x11`, `tag == "ABC"` | Hides padding / adjacent corruption |
| `<leader>dm` / `x/16xb` | Raw bytes at an address | Easy to paste the wrong address |
| Watches on `pt.x` | Pretty value beside bytes | Not a substitute for finding `11 00 00 00` |

**Rejected wrong reading:** "I do not see the integer eleven as ASCII characters
`11`." You are hunting hex byte `11`, not digit characters.

## `<leader>dm` workflow (anchors)

1. Stop with Scopes showing `pt` / `arr` / `p`.
2. Copy an address (`&pt`, `p`, or `arr` — the **pointer value**, not the string
   `"pt"` and not the pretty integer `0x11`).
3. `<leader>dm` — paste into the float memory viewer (or use the bottom memory
   panel).
4. Scan for the patterns above. Then memory-view `arr` separately if the UI
   starts at `&pt` only.
5. Add a Watch on `pt.x` or `arr[2]` if you want the pretty value beside the
   bytes.

### Where to break

| Break location | Pros | Cons |
| -------------- | ---- | ---- |
| Start of `fill_demo` | Watch fills appear as you F10 | Locals may be uninitialized at the very first line |
| Just after `fill_demo` returns in `main` | `pt` / `arr` in `main` stay live; stable photograph | You miss the paint process |

After-return is often easier for a first photograph because locals in `main`
stay live for Scopes.

gdb twin commands: `x/16xb &pt`, `x/4wd arr`, `p pt`. Same bytes, different
keyboard. Lesson 35 used this skill on `$rsp`; here the target is a struct you
filled yourself, which makes the expected pattern knowable in advance.

## Keys for this lab

| Key | Role |
| --- | ---- |
| F9 | Break at fill start or after return in `main` |
| F5 / `<leader>dc` | Launch existing → `./prog` |
| F10 | Step through fills if you broke early |
| `<leader>dm` | **Primary** — paste `&pt` / `arr` |
| `<leader>de` | Hover fields for pretty cross-check |
| `<leader>du` | Recover panes |
| `<leader>dt` | Terminate |

## Worked example

**The situation.** You stopped after `fill_demo` filled `pt` and `arr`.

**Step 1 — struct bytes.** Memory at `&pt`: find `11 00 00 00`, then
`22 00 00 00`, then `41 42 43 00`. If you see `00 00 00 11`, you are either on
the wrong endian interpretation or looking at a different word — re-check you
started at `&pt` and are reading LE.

**Step 2 — array words.** Memory at `arr`: four little-endian ints 10..40. The
rejected wrong reading is: "array elements are adjacent one-byte fields." They
are 4-byte `int`s unless you declared a different type.

**Step 3 — cross-check.** Watch `pt.x` or `p pt` matches the first word you
saw. When pretty and raw disagree, raw wins for layout bugs; pretty wins for
speed once you trust layout. If `tag` shows `ABC` in Scopes but memory lacks
`43`, you are looking at the wrong address or an unfilled buffer.

**Step 4 — optional gdb.** `x/16xb &pt` and `x/4wd arr` should match the dap
float byte-for-byte.


## Endianness traps in one paragraph

Humans write `0x11` with the "eleven" feeling on the right. Little-endian
memory stores the least significant byte first: address `N` holds `0x11`,
address `N+1` holds `0x00`, and so on for a 32-bit int. Big-endian would store
`00 00 00 11`. This track's x86-64 Linux targets are little-endian — if your
dump shows `00 00 00 11` at `&pt.x` after a correct fill of `0x11`, you are
probably not at `&pt` or you filled wrong.

ASCII is unaffected by endianness byte-swap myths: `'A'` is always `0x41` in
the byte you wrote. The four `tag` bytes are simply consecutive characters.

### Cross-check recipe when patterns refuse to appear

1. Confirm `./prog` prints `ok` — otherwise you may be staring at zeros from an
   unfinished `fill_demo`.
2. In Scopes, expand `pt` and confirm pretty values (`0x11`, `0x22`, `ABC`).
3. Hover `&pt` / `p` / `arr` and re-copy the address at *this* stop (pointer
   values can look similar across runs under ASLR — copy fresh).
4. `<leader>dm` again. If still wrong, `x/16xb &pt` in gdb as a second opinion.
5. Only then consider packing/`sizeof` surprises — for this struct at `-O0` on
   this ABI, the simple layout above should hold.

**Rejected wrong reading:** "Memory view is broken because Scopes looks fine."
Usually the anchor address is wrong.



## Relating memory view to smash and stack labs

Lesson 37's smash used the same `<leader>dm` key on `&buf` to see `0x41` runs.
Lesson 35 used it on `$rsp` to see return addresses. This lesson uses it on a
*well-known* layout you filled yourself so the expected pattern is knowable
before you open the viewer. Once that recognition is solid, smash dumps and
stack dumps stop being abstract hex.

If you can find `11 00 00 00` / `41 42 43 00` / `0a 00 00 00` cold, you are
ready to trust raw memory over a lying pretty-printer elsewhere in the track.


## Distinctions worth keeping straight

- **Pretty local vs raw bytes** — Scopes versus `<leader>dm` / `x/`.
- **Little-endian int vs ASCII digit characters** — `0x11` versus `'1','1'`.
- **Address to paste vs value of `pt.x`** — `&pt` versus `0x11`.
- **Break inside fill vs after return** — watching paint versus final photo.
- **Struct field order vs memory order** — source order matches layout here;
  packing labs elsewhere will break that comfort.

## Check yourself

1. In little-endian memory, which byte of `pt.x` holds `0x11`?
2. What key opens the dap memory float?
3. How wide is each `arr[i]` in bytes for `int arr[4]`?
4. Why might after-return be easier than breaking on the first line of
   `fill_demo`?
5. Scopes shows `pt.x = 0x11`. Do you paste `0x11` or `&pt` into `<leader>dm`?
6. What ASCII hex sequence corresponds to `"ABC"` with a trailing NUL?

If fuzzy, re-read the expected-bytes table before `help x`.

## Key takeaways

- `<leader>dm` turns addresses from Scopes into readable bytes.
- Anchor on `&pt` / `arr` — never paste a field's *value* as if it were an
  address (unless that value really is a pointer).
- Little-endian ints and ASCII tags are recognizable patterns once you know
  what to hunt.
- gdb `x/` is the same skill with different spelling.
- Layout bugs show up in raw memory even when a single field print looks fine.

## Lookup (not the lesson)

- `NVIM_DAP.md` — `<leader>dm`, Watches, memory panel
- `man 1 gdb` — `help x`

Now open `TASK.md` and do the practice.
