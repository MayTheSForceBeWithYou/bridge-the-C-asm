# Extended prompt for Grok Bot: expand `c-and-asm-grind`

Paste everything below this line to Grok.

---

You previously built `/home/n8/dev/c/c-and-asm-grind` for me from a prompt in
`PLAN_PROMPT.md` in this same directory — read that file first, it's still
accurate background. You produced 31 numbered exercises (`01-hello-pipeline`
through `31-conditional-move`): Track A (write C, inspect the compiler's
asm) and Track B (hand-write GNU `as`, link against a C test harness). No
solutions anywhere, self-contained per-directory Makefiles via `common.mk`,
docs pointers restricted to what's verified in `REFS.md`. That structure was
good. This is not a rewrite — **do not touch the 31 existing exercise
directories except for the two targeted fixes in §1.** Everything else here
is pure addition: new tracks, more reps, more depth.

I have a lot of usage credits earmarked for you specifically on this
project. Do not conserve effort. Go wide and go deep — more exercises per
track than you think I need, more angles on the same underlying mechanism,
more "and now do it again with the next variable changed." Reps over
reading is still the philosophy; there should just be a lot more reps.

## Inspiration: Handmade Hero

Casey Muratori's *Handmade Hero* is the north star for this expansion. Pull
these specific habits from it into the new exercises, concretely, not as
vibes:

- **Nothing is magic.** Every layer between your keystroke and the CPU
  executing an instruction gets opened up at some point: the compiler, the
  assembler, the linker, the loader, the OS syscall boundary, the memory
  system. If an exercise would normally stop at "and libc handles the
  rest," that's exactly where a new exercise should start.
- **The debugger is a primary tool, not a last resort.** Casey lives in a
  live debugger, stepping instruction by instruction, watching registers
  and memory change in real time as code runs — not just reading a static
  `objdump -d` listing after the fact. The current project has zero
  exercises that use `gdb` interactively. That's the single biggest gap —
  fix it with a full new track (§3).
- **Measure, don't guess, about performance.** Casey's stance on
  optimization is: form a hypothesis, then measure with a cycle counter or
  profiler, then look at what the CPU actually did. Nothing here currently
  measures anything — every existing exercise inspects static asm, never
  runtime behavior under a clock. Fix with a new track (§5).
- **Own your memory.** Handmade Hero avoids hidden allocators and prefers
  large up-front allocations you manage yourself (arenas, bump allocators)
  over scattered `malloc`/`free`. Track B currently stops at `exit` and
  `write` syscalls — extend it to `mmap`/`brk` and a hand-rolled arena
  (§4).
- **Understand what happens before your code runs, and after it ends.**
  Casey cares about the platform layer — what the OS does to get your
  first instruction running, what "process" and "executable" concretely
  mean. The project currently treats the linker and loader as a black box
  (`ld` gets one line in `REFS.md` and is otherwise unused). Fix with a new
  track (§4b — ELF/linking).
- **Bits are not abstract.** Union punning, fixed-point math, tagged
  pointers, endianness — Casey treats the bit pattern as the ground truth
  and the C type as a convenient lie on top of it. New track (§6).

## §1 — Two targeted fixes to existing exercises

Small, surgical, don't rewrite anything else:

1. **Track A exercises don't self-verify.** Track B harnesses (e.g.
   `24-asm-sum-loop/harness.c`) use `expect()`-style asserts and print `ok`
   or exit non-zero on failure — I get a real pass/fail without having to
   eyeball output. Most Track A `prog.c` files just `printf` two numbers
   and leave me to check by hand (see `07-arrays-vs-pointers/prog.c`).
   Retrofit every Track A exercise's `main` with the same assert-and-`exit`
   pattern Track B already uses, checking the *behavior* (e.g.
   `sum_idx(a,5) == sum_ptr(a,5) == 15`), never revealing the intended
   implementation. Keep the printed summary for exercises where the asm
   *shape* is the real point (e.g. 31's `cmov` check has no single "right"
   number to assert on) — use judgment, but default to adding a check.

2. **`REFS.md` cites `addr2line` and `strip` but no exercise uses either.**
   Dead references undercut the "only point at what's real" rule the file
   states about itself. §3 and §4b below add exercises that exercise both;
   after adding them, double check every line in `REFS.md` is cited by at
   least one `TASK.md`, and every doc pointer in every `TASK.md` is listed
   in `REFS.md`. Fix any mismatch either direction.

Also, mechanical fix to the **root `Makefile`**: it hand-lists all 31
exercise names twice (once in `EXERCISES :=`, once in the numeric-shortcut
block). That was fine at 31 entries; it will not stay fine once this
expansion roughly triples the exercise count. Replace the hand-maintained
list with directories discovered by pattern
(`$(sort $(patsubst %/,%,$(wildcard [0-9][0-9]-*/)))` or equivalent) so
adding a new numbered directory never requires editing the Makefile again.
Keep the `make NN` numeric-shortcut ergonomic — generate those targets from
the same discovered list instead of hand-writing 60+ lines.

## §2 — Environment additions (verified on this machine, do not re-guess)

Everything in `PLAN_PROMPT.md`'s environment section still holds. Additions
specific to this expansion, already checked on this exact box so you don't
need to hedge:

- `gdb` is GDB 17.2, interactive TUI mode works (`layout asm`, `layout
  regs`, `layout split`).
- `addr2line`, `strip`, `nm`, `objdump`, `readelf`, `ldd` are all present
  (binutils 2.47).
- `strace` and `perf` are **NOT installed**, but both are in the Arch
  `extra` repo under their own names: `pacman -S strace` and
  `pacman -S perf` (the `perf` package is part of `linux-tools`). Any
  exercise that wants them must (a) check with `command -v strace` /
  `command -v perf` and print a one-line "optional: `sudo pacman -S
  strace`" hint rather than hard-depending on it, and (b) have a
  fallback path that works without it (gdb + hand-rolled `rdtsc` timing
  cover everything strace/perf would add, just with more manual work —
  that's fine, it's more reps).
- `L1_DCACHE_LINESIZE` on this CPU is 64 bytes (`getconf
  LEVEL1_DCACHE_LINESIZE`) — exercises can state this as a known fact
  rather than making me discover it, since it's a hardware constant, not
  something an exercise should burn a rep on.
- `rdtscp` and `constant_tsc` are both present in `/proc/cpuinfo` — cycle
  counting with `rdtscp` (not plain `rdtsc`, use the serializing variant)
  is reliable here.
- `gcc -march=native` resolves successfully — safe to use for
  autovectorization exercises.
- `-lpthread` links cleanly — safe to use for the false-sharing exercise.
- GCC is version 16.2.1 — recent enough for `-fopt-info-vec-optimized` and
  `-fanalyzer` if either becomes useful; verify exact flag spelling
  yourself against `man 1 gcc` on this machine rather than trusting
  version-specific flag names from memory, same rule as always.

## §3 — New Track C: live debugging with gdb (target: 8 exercises, `32`–`39`)

Everything here happens *while the program runs*, not after, on a real
`gdb` session I drive interactively. Every `TASK.md` should give the exact
`gdb` command sequence to type (breakpoints, `stepi`/`nexti`, `info
registers`, `x/` memory examine syntax) as the "Do" steps — that's not
giving away a solution, it's teaching the tool, exactly like Track A's
`TASK.md`s name the `objdump` flags to use.

Progression ideas (adjust freely, these are seeds not a spec):

- Break at the entry of a Track A function you already wrote (e.g. the
  `sum_idx`/`sum_ptr` pair from exercise 07), `stepi` through the
  prologue, and confirm live in `info registers` that arguments really do
  arrive in `rdi`/`rsi` before a single instruction reads them.
- Same idea against a **Track B** hand-written routine (e.g.
  `22-asm-add3`) — confirm your own hand-written prologue matches the
  ABI by watching registers, not by re-reading your own `.s`.
- Use a watchpoint (`watch`) on a stack local at `-O0` vs `-O2` and observe
  it silently fail to trigger at `-O2` because the variable got promoted
  to a register — connects directly back to Track A exercise 03
  (locals/stack).
- `x/8xg $rsp` immediately after a `call` instruction to see the return
  address sitting on the stack; single-step past a few `push`es in a
  Track B routine and watch the stack pointer move by exactly the amount
  you'd predict.
- TUI mode (`layout regs`, `layout asm`) stepping through a loop
  (Track A exercise 08 or Track B exercise 24) instruction-by-instruction,
  watching a register increment in real time.
- Deliberately smash a stack buffer (small fixed array, write past the
  end) and watch gdb show the corrupted return address / the crash at an
  unexpected instruction — connects to why the ABI and stack layout
  studied earlier actually matter, not just as trivia.
- Take a **stripped** binary (`strip` a copy of an earlier exercise's
  binary) and, using only `objdump -d` with no symbols plus `gdb` set to
  break on a raw address, find and annotate a function's boundaries by
  hand. This is the exercise that finally uses `strip` from `REFS.md`.
- Crash a program on purpose (NULL deref), get a backtrace address, and
  use `addr2line -e <binary> <address>` to map it back to the source line
  — the exercise that finally uses `addr2line` from `REFS.md`. Compare
  against what `gdb`'s own `bt` shows for the same crash, and explain any
  difference (debug info presence, inlining).

## §4 — New Track D: syscalls beyond exit/write, owning your memory
(target: 7 exercises, `40`–`46`)

Track B currently stops at `exit` and `write`. Extend it toward the
Handmade-Hero "own your memory, don't trust a hidden allocator" ethos —
still no libc `malloc` anywhere in this track.

- `open`/`read`/`close` a real file via raw syscalls in hand-written
  `.s` (no libc) — read the exercise's own `TASK.md` off disk and print
  its contents back out via the raw `write` syscall you already built in
  exercise 26.
- `mmap` a region directly via syscall (not `malloc`), write a pattern
  through the returned pointer, read it back, `munmap` it. No libc
  allocator involved anywhere in this exercise.
- `brk`/`sbrk`: observe the classic heap-growth syscall directly: call
  `brk(0)` to get the current break, `brk(new)` to grow it, read/write
  into the new region. Contrast with what a large `malloc()` call actually
  does under the hood (this is where `strace`, if the student installed
  it per §2, becomes genuinely useful — make that connection explicit in
  the `TASK.md` as an optional deeper look, with the no-strace fallback
  being: read `man 2 brk`/`man 2 mmap` and reason about it from the glibc
  malloc source comments or `info libc` malloc internals node instead).
- A `SIGSEGV` handler installed from C (`sigaction`), deliberately
  dereference a NULL/bad pointer, and have the handler print something and
  exit cleanly instead of crashing raw — pairs with Track C's crash
  exercise; this time the "crash" is caught deliberately.
- A minimal bump-pointer arena: `mmap` one big block up front in C, hand
  out slices from it with a simple bump pointer and no `free` at all, and
  use it to allocate nodes for a rebuilt version of the exercise 17 linked
  list — no `malloc`/`free` calls anywhere in the arena or the list code.
- Raw `argc`/`argv`/`envp` access from a **Track B, no-libc `_start`**
  entry point — read them straight off the initial stack layout the
  kernel hands you at process start (no C `main` prologue doing it for
  you), and print `argv[0]` via your own `write` syscall wrapper.
- Stretch: chain two of the above — an arena-backed program that reads
  its own source file via raw syscalls, counts something in it (e.g.
  newline count), and reports the result via raw `write`, with zero libc
  calls in the whole binary (`-nostdlib`, verify with `nm`/`readelf` that
  no libc symbols are pulled in).

## §4b — New Track E: ELF, linking, and what runs before `main`
(target: 8 exercises, `47`–`54`)

`ld` is in `REFS.md` and currently unused by anything. This track opens up
the "linker and loader are not magic" layer.

- Static vs dynamic build of the same hello-world: compare `ldd` output,
  binary size (`size`), and `readelf -d` dynamic section presence/absence.
- Disassemble a call to `printf` in the *dynamically* linked build and
  find the `.plt` stub the call actually jumps to first; read `objdump -d
  -j .plt` and explain lazy binding from what you see, citing `info ld` or
  `man 5 elf` for the mechanism name.
- `readelf -h` comparison of a PIE binary (default) vs one built with
  `-no-pie`: `Type: DYN` vs `Type: EXEC`. Run the PIE one twice under
  `gdb` (`info proc mappings` or `print $rip` at a breakpoint) and observe
  the base address change between runs — that's ASLR, made concrete
  instead of a word in a slide.
- `nm`/`readelf --syms` on a binary built with `-fvisibility=hidden` vs
  the default, to see which symbols disappear from the dynamic symbol
  table.
- Find and disassemble `_start` in a real binary
  (`objdump -d -j .text <binary> | grep -A20 '<_start>:'`), and trace by
  hand (reading, not running — this pairs with the gdb version if you want
  to add it) how it gets from the kernel's initial jump to `main` being
  called with `argc`/`argv` ready. Cite `info libc` for
  `__libc_start_main` if the local docs cover it; otherwise say plainly
  that this one goes past what's in `REFS.md` and is read-only reasoning
  from the disassembly.
- `strip` a copy of an earlier binary, confirm it still runs correctly,
  and diff `nm` before/after (symbols gone) and `size`/file-size before/
  after (smaller). This is the *other* exercise that finally exercises
  `strip` alongside the one in Track C — fine to have both, they hit
  different tools around it (`nm` here vs `gdb`+objdump-only reversing in
  C).
- A hand-written `_start`-based `-nostdlib` program (build on Track D's
  no-libc work) linked with an explicit `ld` invocation instead of
  letting `gcc` drive the link — see the raw `ld` command line that `gcc
  -v` would normally hide, and explain each flag from `man 1 ld`/`info
  ld`.
- Optional stretch, mark it clearly optional: a minimal custom linker
  script (`info ld`'s linker-script node) that places a custom section at
  a chosen address, verified with `readelf -S`. Only include if you're
  confident the `TASK.md` can specify a checkable success condition
  without handing over the script itself.

## §5 — New Track F: performance — measure, don't guess
(target: 8 exercises, `55`–`62`)

Nothing in the project currently runs a clock. This track fixes that, and
is the one most directly lifted from Handmade Hero's practice.

- Hand-roll a cycle counter using `rdtscp` (inline asm or a tiny `.s`
  helper, your choice which — either is legitimate given Track B already
  covers inline asm at exercise 19), and time a simple loop. State
  clearly that raw cycle counts vary run to run and the exercise is about
  relative comparison, not an absolute "right" number.
- Time the *same* loop compiled at `-O0` vs `-O2` vs `-O3 -march=native`,
  and require the student to connect the cycle-count difference back to
  the actual instruction-count/shape difference already visible via
  `objdump -d` (tie explicitly back to Track A methodology — this
  exercise is Track A's technique plus a clock).
- Autovectorization: construct a loop `-O3` vectorizes (look for `xmm`/
  `ymm` and packed mnemonics like `paddd`/`vaddps` in the disassembly, or
  use `-fopt-info-vec-optimized` if that flag checks out on this GCC) and
  a superficially similar one it refuses to vectorize because of a
  potential aliasing hazard; fix the second with `restrict` and confirm
  the asm changes.
- Branch misprediction: the classic "sum only values over some threshold"
  loop over sorted vs shuffled data of the same content, same length —
  measure both with your own `rdtscp` timer, and explain the gap by
  referring back to Track A exercise 31 (`cmov` vs real branches) —
  a sorted-array version may let the compiler or CPU predict branches near
  a perfect rate, the shuffled one won't.
- Cache line effects: sequential vs strided (stride = one cache line, 64
  bytes per §2) array traversal over the same total data volume, timed.
  Explain from the 64-byte cache line fact given in §2 — no need to
  "discover" that number, it's stated, the point is applying it.
- False sharing: two `pthread`s each bumping their own counter, once as
  adjacent `int`s in one cache line and once padded to separate cache
  lines, timed both ways. `-lpthread` is confirmed available.
- If `perf` got installed per the optional §2 hint, an optional bonus
  version of two of the above using `perf stat -e
  branch-misses,cache-misses,instructions,cycles` to cross-check the
  hand-rolled `rdtscp` numbers against real hardware counters. Must have
  a working non-perf path already (the exercises above), this is
  strictly additive.
- **Capstone of this track:** a naive scalar loop is given (something with
  an obvious branch-heavy or cache-unfriendly pattern). The task: apply
  what was learned across this whole track — restructure it to be
  branchless/cache-friendly/vectorizable — and beat the *compiler's own*
  `-O2` baseline on your own `rdtscp` measurement of the *original* naive
  version built at `-O2`. This is the single most Handmade-Hero exercise
  in the set: hypothesize, change code, measure, compare against ground
  truth you established yourself.

## §6 — New Track G: bits, unions, and type games
(target: 6 exercises, `63`–`68`)

- Type-pun a `float` through a `union` with a `uint32_t`, print the raw
  bits in hex, and verify by hand against IEEE-754 single-precision layout
  (sign/exponent/mantissa) computed on paper — this is a `[TRACE]`-style
  hand computation before any code runs, don't let the program just spit
  out the answer without the student predicting it first.
- The same type pun done via `memcpy` instead of a union (the
  standard-compliant alternative) — compare `-O2` disassembly of both
  approaches and confirm they collapse to identical code, addressing the
  "union punning is UB in strict C, memcpy isn't" distinction directly.
- A tagged-pointer trick: given that `malloc`/heap allocations are at
  least 8-byte aligned, stash a 3-bit tag in an 8-byte-aligned pointer's
  low bits, extract it back with mask/shift, and inspect the generated
  `and`/`or` asm.
- Bitfield struct vs the manually-packed version from exercise 20
  (alignment/packing): compare `sizeof` and the mask/shift asm generated
  for a bitfield member access against a hand-rolled shift-and-mask
  accessor doing the same thing.
- Endianness made concrete: read a known multi-byte constant through a
  `char*` on this (little-endian x86-64) machine, confirm byte order by
  eye, then use `__builtin_bswap32`/`bswap` (check the actual mnemonic
  GCC emits) to flip it and re-verify.
- A minimal Q16.16 fixed-point type: multiply and divide two fixed-point
  values using integer shifts (no float instructions anywhere), inspect
  the `sar`/`imul`-based asm, and connect it back to Track A exercise 16
  (float/`xmm`) by contrast — this is the "avoid floats on a Handmade-Hero-
  era target" idiom, made concrete rather than asserted.

## §7 — Numbering and organization

Continue the existing flat numeric scheme (`32-...` through `68-...` given
the exercise counts above, adjust if you add or cut any) rather than
switching to a lettered/track-prefixed scheme — it preserves the `make NN`
shortcut ergonomics already built into the root Makefile, and the fix in
§1 makes maintaining that list automatic instead of hand-edited. Group by
track in the **README's table only** — add a "Track" column (A/B/C/D/E/F/G)
to the existing table so the numeric IDs stay flat but the conceptual
grouping is still visible at a glance. Update the top-level `README.md`'s
philosophy section to mention live debugging and measurement as
first-class alongside "look at real output" — they're the same idea
applied to runtime behavior instead of static asm.

## §8 — Deliverable

Same rules as the original prompt: write out the full directory tree and
file contents now for every new exercise (`TASK.md`, starter `.c`/`.s`
files, any harness needed) — no solution bodies, only stubs, signatures,
harnesses, and TODOs for me to fill in. Apply the two targeted fixes in
§1. Update `REFS.md` with any new doc pointers the new tracks introduce
(verify each one resolves on this machine before citing it — same
standard the file already holds itself to), and update the root
`Makefile`/`README.md` per §1 and §7. Do not modify any existing
exercise's `.c`/`.s`/`TASK.md` content beyond what §1 explicitly asks for.
