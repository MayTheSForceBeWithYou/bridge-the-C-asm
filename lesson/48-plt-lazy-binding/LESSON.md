# 48 — PLT stubs and lazy binding

When a dynamic binary calls `printf`, the call instruction usually targets a stub in
the Procedure Linkage Table (PLT), not the final libc address. The first call asks the
dynamic linker to resolve the symbol and rewrites a Global Offset Table (GOT) entry so
later calls jump straight there. This lesson teaches you to *see* that stub and name the
mechanism.

## What this lesson asks of you

Build a dynamic hello that calls `printf`. With `objdump -d -j .plt` and `readelf`,
locate `printf@plt`, describe what it jumps through, and explain lazy binding in your
own notes. Optional gdb (break, call twice) makes first-vs-later vivid.

## PLT and GOT in one paragraph

The PLT stub is a tiny reachable trampoline in your binary. It loads or jumps via a GOT
slot that initially points back into the resolver path. On first use, the dynamic linker
looks up `printf` in libc and updates the GOT. Lazy binding means "resolve on first
call," which is why the first invocation can look different under the debugger than the
second.

`readelf -r` shows relocations related to this; `readelf -d` shows dynamic tags. You do
not need every relocation type memorized — you need the call story.

## How to look

```
objdump -d -j .plt ./hello
objdump -d ./hello | less   # find call toward printf@plt
readelf -r ./hello
readelf -d ./hello
```

In `main`, expect `call printf@plt` (AT&T spelling may vary slightly). In `.plt`, expect
a short sequence ending in a jump through a GOT-related address.

## Worked example

**The situation.** You found the address of `printf@plt` and wrote: first call resolves;
later calls use the filled GOT.

**Step 1 — find the stub.** `objdump -d -j .plt` lists stubs. Match the one named for
`printf`. The rejected wrong reading is: "the address in `call` is already the final
libc address baked at link time." For dynamic lazy binding, the call targets the PLT
stub in *your* text, not the ultimate libc VA (which moves with ASLR anyway).

**Step 2 — name the indirection.** The stub uses the GOT. The rejected wrong reading is:
"the GOT is only for global data variables." Function symbols use GOT slots too for PLT
binding.

**Step 3 — optional gdb.** `break printf`, `run`, dig around, `finish`, call again —
observe whether you still stop the same way. Illuminating, not required for credit if
you already explained lazy binding from the dump.

## Distinctions worth keeping straight

- **PLT stub vs final function body** — trampoline versus libc implementation.
- **Lazy vs now binding (`LD_BIND_NOW`)** — resolve on first call vs at startup.
- **Relocation records vs runtime addresses** — metadata versus filled GOT after
  resolve.
- **Static binary** — often no printf PLT; the call may be direct. Contrast with 47.

## Check yourself

1. Why does `main` call `printf@plt` instead of an absolute libc address?
2. What gets updated on the first call under lazy binding?
3. Which `objdump` flag limits output to the PLT section?
4. How would a fully static hello differ in this inspection?

## Key takeaways

- Dynamic calls to shared functions commonly go through the PLT.
- Lazy binding resolves on first use via the GOT and dynamic linker.
- `objdump -j .plt` and `readelf` make the mechanism inspectable.
- First call and later calls can differ in observable cost/path.

## Lookup (not the lesson)

- `man 1 objdump`, `man 1 readelf`, `man 5 elf`, `info ld`, `man 1 gdb`

Now open `TASK.md` and do the practice.
