# 72 — `main` in asm, linked with libc

Freestanding `_start` programs talk to the kernel. Most real programs still
want libc: `puts`, `printf`, heap, and a civilized exit. This lesson has you
write all of `main` by hand while CRT and `-lc` stay in the link — so you meet
their contract from the callee side of `main` and the caller side of libc. By
the end you should explain who calls `main`, fix `%rsp` alignment before
`puts`/`printf`, and zero `%al` for a variadic call with no XMM args.

## What this lesson asks of you

Provide a global `main` in `hello_asm.s` with no C translation unit of your
own. Call `puts` and `printf`, then return 0 so CRT exits cleanly. Deliberately
break stack alignment once and watch where it dies. Compare your prologue to
`gcc -S` from exercise 01.

## Who calls `main`?

The ELF entry is still CRT `_start` (names vary by libc). That path sets up
TLS, libc internals, and eventually calls `main`. Your return value in `%eax` /
`%rax` becomes the process exit status after CRT runs `exit`-family cleanup.

So you are not freestanding: you must return (or `call exit`), keep ABI
alignment for libc calls, and use the same argument registers as any C function.

**Navigation in gdb:** `gdb -q ./hello_asm`, then `break main`, `run`, `bt`.
You should see CRT frames *above* `main` — useful proof that you did not become
`_start`.

**Rejected wrong reading:** "Because I wrote `main` in asm, I am freestanding
and must `syscall` exit." Linking with CRT means return-from-`main` is the
civilized path (exercise 21 remains the freestanding story).

## Alignment at the first instruction of `main`

| Moment | `%rsp` mod 16 | Why |
| ------ | ------------- | --- |
| First insn of `main` | 8 | CRT's `call main` pushed a return address |
| At your `call puts` / `call printf` | must be 0 | ABI rule before every `call` |
| After `push %rbp` | 0 | typical one-instruction fix |
| After further `sub $N, %rsp` | 0 if `N` multiple of 16 | room for locals |

If you strip the prologue pushes/subs and call libc at `-O0`, many builds die
inside the callee with a general-protection or segmentation fault when an SSE
spill uses a misaligned stack slot. **Recognition rule for the symptom:** note
*which* libc routine appears in the backtrace — that is evidence the bug was
alignment, not a bad format string. A missing symbol would have failed at
**link** time, not at run time inside `printf`.

**Rejected wrong reading:** "Crashing inside `printf` means my format string is
wrong." Often your stack was wrong; the format string never got a fair chance.

## Talking to `puts` and `printf`

Decode the outbound ABI the same way as exercise 27:

| Call | `%rdi` | `%rsi` | `%rdx` | Extra |
| ---- | ------ | ------ | ------ | ----- |
| `puts(greeting)` | pointer to greeting | — | — | — |
| `printf(fmt, greeting, 14)` | format pointer | greeting pointer | integer 14 | **`%al` = number of XMM args** |

Variadic functions also look at `%al` for the *number of vector (XMM)
arguments*. For a call with no floating-point args, zero `%al` (often
`xor %eax, %eax` before `call printf` when you do not otherwise need `%rax`).
Leaving `%al` garbage can make `printf` touch random XMM state and crash or
corrupt.

**Rejected wrong reading:** "Variadic is just like three normal args; `%al`
does not matter." On this ABI it does.

**Rejected wrong reading:** "`%al` is the total argument count." It counts
*vector* arguments, not integer/pointer args in GPRs.

String literals belong in `.rodata` (exercise 74 drills directives). For this
lesson, follow the stub's labels for `greeting` and `fmt`.

### Loading a string address (preview of 75)

In a PIE build you typically see `lea greeting(%rip), %rdi` — RIP-relative —
not `mov $greeting, %rdi`. If your stub already uses the RIP-relative form,
keep it; exercise 75 explains why absolute immediates fail to link as PIE.

## What `make` maps to

| You type | Result | Open / note |
| -------- | ------ | ----------- |
| `make && make run` | linked with CRT + libc | expected two stdout lines; exit 0 |
| `make disasm` | `objdump -d` saved | search `<main>:` |
| Break alignment; rerun | often SIGSEGV | record which frame in `bt` |
| Compare to ex. 01 | C `main` prologue | same contract, different author |

## How to find *your* `main` without drowning in CRT

1. In the listing, **search `<main>:`** (angle brackets and colon).
2. Treat `_start`, `.plt`, and friends above it as runtime glue (Track E / earlier
   exercises).
3. Inside `main`, identify prologue (`push %rbp` / `mov %rsp,%rbp` / optional
   `sub`).
4. Find `call … <puts@plt>` (or similar) — left column = call site; hint = PLT
   stub toward libc.
5. Find `call … <printf@plt>` nearby; just above it, look for a zeroing of
   `%eax` / `%al`.
6. Confirm a zeroing of `%eax` before `ret` for status 0.

Example shape (addresses vary; columns do not):

```text
    11a2:	e8 89 fe ff ff       	call   1030 <puts@plt>
    ...
    11b0:	31 c0                	xor    %eax,%eax
    11b2:	e8 99 fe ff ff       	call   1050 <printf@plt>
```

| Token | Meaning |
| ----- | ------- |
| `11a2:` | instruction address of the `puts` call |
| `<puts@plt>` | PLT stub — not the final libc address |
| `xor %eax,%eax` before `printf` | clears `%al` for zero XMM args |
| Do not confuse with status return | you may `xor` again later for `return 0` |

**Rejected wrong reading:** "The first `call` in the file is my `puts`." Anchor
on `<main>:` first — CRT calls earlier.

## Worked example

**The situation.** Stub `hello_asm.s` links and may exit 0 silently until you
implement the calls.

**Step 1 — frame + `puts`.** Build a minimal aligned prologue, load the
greeting address into `%rdi`, `call puts`. Expected line: `hello from asm`. The
rejected wrong reading is: "I can pass the string in `%rax` because that is the
return register." Callees read `%rdi` for the first pointer arg.

**Step 2 — `printf` with `%al` cleared.** Set up three integer/pointer args and
zero `%al`, then `call printf`. Expected second line mentions `14` chars.

**Step 3 — return 0 and compare.** `xor %eax, %eax` (or `mov $0, %eax`),
epilogue, `ret`. Run `make disasm` and place your prologue next to exercise 01's
C `main` — same contract, different author.

**Step 4 — break alignment on purpose.** Remove `push`/`sub`, rebuild at `-O0`,
run under gdb, `bt` on the fault. Record the top frames. Restore the prologue
afterward so `make run` is green again.

## Distinctions worth keeping straight

- **`main` vs `_start`** — CRT entry versus the C-level function you write here.
- **Return from `main` vs `syscall` exit** — CRT turns your return into process
  status; freestanding code cannot lean on that.
- **`%al` for variadic vector count vs integer arg count** — integers live in
  GPRs; `%al` is not "number of total args."
- **Alignment symptom vs wrong symbol** — crashing inside `printf` often means
  your stack; a missing symbol fails at link.
- **Call site address vs `@plt` hint** — where the call lives vs where it goes
  next.
- **`xor %eax,%eax` before `printf` vs before `ret`** — different jobs (XMM
  count vs exit status).

## Check yourself

1. What is `%rsp` mod 16 at the first instruction of `main`, and why?
2. Which register tells `printf` how many XMM args were passed?
3. Who calls `main`, and where does your return value go next?
4. Why might removing `push %rbp` alone cause a fault inside libc?
5. In a listing, what search string takes you to your code rather than `_start`?
6. On a `call … <printf@plt>` line, which token is the instruction address?

## Key takeaways

- Asm `main` still obeys the C ABI; CRT calls you and consumes your `%eax`.
- Align `%rsp` before every libc `call`; entry skew is 8 from the return
  address.
- Zero `%al` before variadic calls with no SSE args.
- Broken alignment often faults in the callee — use that when debugging.
- Find `<main>:` before interpreting the first interesting `call` in the file.

## Lookup (not the lesson)

- `man 3 puts`, `man 3 printf`, `man 1 as`, `man 1 gcc`, `info as`
- Argument registers from your ex. 04–05 `.s` output
- `man 1 objdump`, `man 1 gdb` — listing and backtrace

Now open `TASK.md` and do the practice.
