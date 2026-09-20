# 47 — Static versus dynamic linking

The same `hello.c` can become a small binary that needs `libc.so` at runtime, or a
large binary that carries its own copy of library code. Those choices show up in
`ldd`, `size`, and `readelf -d`. This lesson trains you to *read* those artifacts so
"dynamic" and "static" stop being slogans.

## What this lesson asks of you

Build a tiny hello both ways (dynamic always; static or static-pie if the toolchain
allows). Fill a comparison: who has an interpreter / dynamic section, who is larger,
and why. If static link fails on this Arch box, document the linker error and go deep
on the dynamic binary anyway.

## Dynamic executables

Default `gcc hello.c -o hello` produces a dynamically linked ELF. `ldd` lists shared
libraries. `readelf -d` shows tags such as `NEEDED` (libraries) and often `INTERP`
(the dynamic linker path, also visible in the program headers). The on-disk file stays
small because libc's bytes live in a shared object mapped at runtime.

Calls to `printf` typically go through the Procedure Linkage Table (PLT) — Track E
exercise 48 zooms in. Here you only need to notice that dynamic machinery exists.

## Static and static-pie

`gcc -static` asks for a fully static link: libc archives are copied in. The binary
grows a lot; `ldd` should report that it is not a dynamic executable. `readelf -d` may
be empty or minimal — no `NEEDED` libc.

On some Arch setups, static glibc pieces are missing or awkward. Fallbacks:

- try `gcc -static-pie`
- if both fail, keep `hello.dyn`, record the error, and still answer every comparison
  question that dynamic artifacts can support

Static-pie is still "mostly self-contained" but may retain PIE dynamics — read the
headers you actually got rather than assuming the flag name.

## Worked example

**The situation.** You have `hello.dyn` and, if lucky, `hello.static`. You run `ldd`,
`size`, and `readelf -d` on each.

**Step 1 — ldd.** Dynamic shows `libc.so.6` and a loader line. Static says "not a
dynamic executable" (or similar). The rejected wrong reading is: "ldd failed, so the
static binary is broken." For static binaries, that message is success.

**Step 2 — size.** Expect text/data/bss totals much larger for static. The rejected
wrong reading is: "static is larger because of debug info." Debug can add weight, but
the dominant story is *copied library code*.

**Step 3 — readelf -d.** Dynamic has `NEEDED` / `INTERP` related story; static lacks
libc `NEEDED`. Note what you see; cite `man 5 elf` for tag names when you need
spellings.

## Distinctions worth keeping straight

- **Dynamic NEEDED vs static archive members** — runtime dependency versus bytes inside
  your ELF.
- **INTERP** — program interpreter for dynamic loading; static usually has none.
- **`-static` vs `-static-pie`** — different ELF types/policies; inspect, do not guess.
- **Link failure vs conceptual failure** — missing static libs are an environment limit,
  not a reason to skip reading the dynamic side.

## Check yourself

1. Which artifact lists shared library dependencies for a dynamic hello?
2. Why is a static hello usually much larger on disk?
3. What `readelf -d` tags signal dynamic linking?
4. If `-static` fails here, what do you still owe in your notes?

## Key takeaways

- Dynamic binaries defer library bytes to shared objects; static copies them in.
- `ldd`, `size`, and `readelf -d` are the quick triangle for comparison.
- Toolchain gaps may block static link; the mechanisms remain visible on the dynamic
  binary.
- File size differences are mostly policy, not mystery.

## Lookup (not the lesson)

- `man 1 ldd`, `man 1 size`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`

Now open `TASK.md` and do the practice.
