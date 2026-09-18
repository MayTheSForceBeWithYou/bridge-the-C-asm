# C and x86-64 Assembly Grind

Learn-by-doing track: write C, study the asm the compiler emits, hand-write
`.s`, debug live in **nvim-dap** (primary in-editor) and/or **gdb** CLI, own
syscalls/memory, open the ELF/linker layer, measure with a cycle counter, and
play with bits. No tutorials in the exercise folders — only prompts, stubs, and
tools you already have (`man`, `info`, `gcc`, `as`, `ld`, `objdump`, `gdb`,
nvim + CodeLLDB).

Sibling tracks under `~/dev/c/` cover beginner C and RPGs. This one starts past
that and leans on the C↔asm relationship.

## Philosophy

1. **Reps over reading.** Many short exercises beat a few long lectures.
2. **Look at real output.** Every task ends with `objdump`, `nm`, `size`,
   `readelf`, or a live debugger against *your* binary — not prose about what
   they would show.
3. **Live debugging is first-class.** Track C drives interactive debugging —
   **nvim-dap** breakpoints / step / scopes / memory (`NVIM_DAP.md`) and/or
   `gdb` (registers, watchpoints, TUI, stripped binaries) — same “look at
   reality” idea while the program runs.
4. **Measure, don’t guess.** Track F times code with `rdtscp` and ties cycle
   counts back to asm shape and microarchitecture effects.
5. **Docs live in the terminal.** Prefer `man` / `info` / `gdb help` (see `REFS.md`).
   Do not hunt for missing man pages; if it is not in `REFS.md`, discover it from
   compiler output instead (especially ABI register conventions).
6. **No solutions.** Verification = compile/assemble/link + expected exit code or
   stdout documented in each `TASK.md`. Track A/B harnesses print `ok` / exit 0
   when behavior is correct.
7. **Self-contained dirs.** Jump around; each exercise builds with `make -C <dir>`.

## Layout

```
c-and-asm-grind/
  README.md REFS.md NVIM_DAP.md Makefile common.mk .gitignore
  .vscode/launch.json
  PLAN_PROMPT.md EXTENDED_PROMPT.md
  01-hello-pipeline/ ... 75-asm-pie-riprel/
```

- **Track A (01–20, 29–31):** write C → inspect `.i` / `.s` / `.o` / disassembly.
- **Track B (21–28, 72–75):** hand-write GNU `as` AT&T assembly; C harness,
  freestanding, or a whole `main` of your own linked against libc.
- **Track C (32–39, 69–71):** live debugging — nvim-dap breakpoints/step/memory
  and gdb CLI (stepi, watchpoints, TUI, strip, addr2line). Dir names keep the
  `32-gdb-*` prefix for stability; content teaches both tools.
- **Track D (40–46):** syscalls beyond exit/write; mmap/brk; arenas; nostdlib.
- **Track E (47–54):** ELF, PLT, PIE/ASLR, strip, explicit `ld`, optional linker script.
- **Track F (55–62):** `rdtscp` timing, opts, autovec, cache, false sharing, capstone.
- **Track G (63–68):** IEEE bits, memcpy vs union, tags, bitfields, endian, Q16.16.

## Exercise table

| # | Directory | Track |
|---|-----------|-------|
| 01 | `01-hello-pipeline` | A |
| 02 | `02-prologue-epilogue` | A |
| 03 | `03-locals-stack` | A |
| 04 | `04-args-regs` | A |
| 05 | `05-return-values` | A |
| 06 | `06-structs-layout` | A |
| 07 | `07-arrays-vs-pointers` | A |
| 08 | `08-loops-jumps` | A |
| 09 | `09-signed-vs-unsigned` | A |
| 10 | `10-switch-jumptable` | A |
| 11 | `11-globals-sections` | A |
| 12 | `12-string-rodata` | A |
| 13 | `13-recursion-stack` | A |
| 14 | `14-tail-call-opt` | A |
| 15 | `15-fn-pointers` | A |
| 16 | `16-float-xmm` | A |
| 17 | `17-linked-list` | A |
| 18 | `18-volatile-opt` | A |
| 19 | `19-inline-asm` | A |
| 20 | `20-alignment-packing` | A |
| 21 | `21-asm-exit-syscall` | B |
| 22 | `22-asm-add3` | B |
| 23 | `23-asm-stack-scratch` | B |
| 24 | `24-asm-sum-loop` | B |
| 25 | `25-asm-strlen` | B |
| 26 | `26-asm-write-syscall` | B |
| 27 | `27-asm-call-c` | B |
| 28 | `28-asm-factorial` | B |
| 29 | `29-call-clobber` | A |
| 30 | `30-lea-vs-add` | A |
| 31 | `31-conditional-move` | A |
| 32 | `32-gdb-args-live` | C |
| 33 | `33-gdb-asm-add3-live` | C |
| 34 | `34-gdb-watchpoint-opt` | C |
| 35 | `35-gdb-stack-after-call` | C |
| 36 | `36-gdb-tui-loop` | C |
| 37 | `37-gdb-stack-smash` | C |
| 38 | `38-gdb-strip-revive` | C |
| 39 | `39-gdb-addr2line-crash` | C |
| 40 | `40-asm-open-read` | D |
| 41 | `41-asm-mmap` | D |
| 42 | `42-brk-heap` | D |
| 43 | `43-sigsegv-handler` | D |
| 44 | `44-bump-arena-list` | D |
| 45 | `45-asm-argv-nostdlib` | D |
| 46 | `46-nostdlib-arena-wc` | D |
| 47 | `47-static-vs-dynamic` | E |
| 48 | `48-plt-lazy-binding` | E |
| 49 | `49-pie-aslr` | E |
| 50 | `50-visibility-hidden` | E |
| 51 | `51-trace-start-to-main` | E |
| 52 | `52-strip-still-runs` | E |
| 53 | `53-ld-explicit` | E |
| 54 | `54-linker-script-optional` | E |
| 55 | `55-rdtscp-timer` | F |
| 56 | `56-opt-levels-timed` | F |
| 57 | `57-autovec-restrict` | F |
| 58 | `58-branch-mispredict` | F |
| 59 | `59-cache-stride` | F |
| 60 | `60-false-sharing` | F |
| 61 | `61-perf-optional` | F |
| 62 | `62-beat-O2-capstone` | F |
| 63 | `63-float-union-ieee` | G |
| 64 | `64-memcpy-pun-vs-union` | G |
| 65 | `65-tagged-pointer` | G |
| 66 | `66-bitfields-vs-manual` | G |
| 67 | `67-endian-bswap` | G |
| 68 | `68-q16-16-fixed` | G |
| 69 | `69-dap-breakpoints-step` | C |
| 70 | `70-dap-memory-view` | C |
| 71 | `71-dap-asm-step` | C |
| 72 | `72-asm-main-libc` | B |
| 73 | `73-asm-cfi-backtrace` | B |
| 74 | `74-asm-sections-directives` | B |
| 75 | `75-asm-pie-riprel` | B |

## How to build

From the repo root:

```bash
make                 # build every exercise that has a default target
make list            # show discovered exercise dirs
make 01              # shorthand for make -C 01-hello-pipeline
make 32              # Track C, etc. — shortcuts auto-discovered
make 69              # dap breakpoint drill
make -C 01-hello-pipeline
make -C 01-hello-pipeline asm disasm run
make O=2 -C 02-prologue-epilogue asm   # override optimization
make clean
```

Inside an exercise:

| Target        | What it does                                      |
|---------------|---------------------------------------------------|
| `all` / `bin` | link the binary                                   |
| `preprocess`  | `gcc -E` → `.i`                                   |
| `asm`         | `gcc -S -fverbose-asm` → `.s`                     |
| `obj`         | `gcc -c` → `.o`                                   |
| `disasm`      | `objdump -d` → `.lst`                             |
| `disasm-intel`| `objdump -d -M intel` → `.lst.intel`              |
| `run`         | run the binary                                    |
| `clean`       | remove build products                             |

`CFLAGS` default to `-ggdb -Wall -Wextra -O0`. Pass `O=2` (or `CFLAGS=...`) to
compare optimization. Track B/D freestanding exercises use `as` + `ld` or
`gcc -nostdlib` as noted in their `TASK.md`. Some Track F Makefiles add
`LDLIBS += -lpthread`.

**Assembler:** GNU `as` / `gcc -S` only. `nasm` is not assumed.

## Suggested workflow (every exercise)

1. Read `TASK.md`.
2. Open the cited `man`/`info` page from `REFS.md` (and `NVIM_DAP.md` for debug).
3. Fill the TODO stub (C or `.s`).
4. `make` → run → inspect (`make asm disasm`, nvim-dap / `gdb`, `nm`, …).
5. Answer the TASK questions in your own notes — not in the repo.

### Multi-file debug (nvim-dap)

`make` first, then **Launch existing executable** (not “Build current file”).
Details: **`NVIM_DAP.md`**.

## ABI note

There is no man page here for the System V AMD64 calling convention. Discover
argument registers and return-in-`rax` by reading `gcc -S` / `objdump -d` on
exercises 04–05 and the Track B harness contracts. For raw syscalls use
`man 2 syscall`, `man 2 syscalls`, and `__NR_*` in
`/usr/include/asm/unistd_64.h` (or `ausyscall`).
