# 46 — Freestanding wordcount stretch

This stretch combines Track D skills: parse argv, open/read a file, use mmap or BSS as
a buffer, count newlines, and print a decimal integer — all without libc. It is a
checkpoint that you can assemble a tiny useful tool from syscalls alone.

## What this lesson asks of you

Build `wc_nl` freestanding: open a path (argv[1] or default `TASK.md`), count `\n`
bytes, write the count as ASCII decimal plus newline, exit. `nm`/`readelf -d` must show
no NEEDED libc.

## Pieces you already know

- **argv** (exercise 45): path string from the stack.
- **open/read/close/write/exit** (40): move file bytes.
- **mmap** (41) or a large `.bss` buffer: hold contents (or stream and count without
  storing everything — either design works if the count is correct).
- **No printf**: convert an integer to decimal by repeated division, emitting digits into
  a small buffer (reverse digit order carefully).

`readelf -d` on a dynamic-looking binary would show `NEEDED`; a true freestanding
`ld`-linked binary should lack an interpreter/NEEDED libc. Confirm with `nm` that you
did not accidentally link `-lc`.

## Decimal without libc

For count `n`, repeatedly `n % 10` / `n / 10` into a temporary buffer from the end, then
`write` the digit substring. Handle `n == 0` as a single `'0'`. Newline after the number.

## Mapping `make` to commands and files

| You type | Check afterward |
| -------- | --------------- |
| `make` | freestanding `./wc_nl` |
| `./wc_nl TASK.md` | decimal newline count + `\n` |
| `readelf -d ./wc_nl` | no `NEEDED` libc |
| `nm ./wc_nl` | no `printf`/`atoi` |

## Pieces you are integrating

| Skill | From |
| ----- | ---- |
| argv path | exercise 45 |
| open/read/write/close/exit | 40 |
| mmap or BSS buffer | 41 / data |
| decimal without printf | this stretch |

**Rejected wrong reading:** "I'll call `printf("%d\n", n)` just for the count." That
pulls libc and fails the stretch.

## Decimal emission algorithm (recognition)

Repeated `n % 10` / `n / 10` into a temporary buffer from the end, then `write` the digit
substring. Handle `n == 0` as a single `'0'`. Append newline.

**Rejected wrong reading:** "count words like `wc -w`." This tool counts `\n` bytes only.

## Proving freestanding

**Navigation:** `readelf -d ./wc_nl` → search `NEEDED`. `nm ./wc_nl` → search `printf`.
A true `ld`-linked freestanding binary should lack libc `NEEDED`.

## Worked example

**The situation.** `./wc_nl TASK.md` prints a number matching `grep -c` style newline
count for that file (count of `\n` bytes). `readelf -d` shows no libc NEEDED.

**Step 1 — open the path.** From argv or default. Failure exits non-zero. The rejected
wrong reading is: "I can call `atoi`/`printf` just for the count." That pulls libc and
fails the stretch goal.

**Step 2 — count.** Either map/read whole file or loop `read` and tally. The rejected
wrong reading is: "count characters that look like words." This tool counts newline
bytes only.

**Step 3 — emit decimal.** Write digits + `\n`. Verify against a known file. Confirm
freestanding with `nm`/`readelf`.

## Streaming versus full-buffer designs

Both are valid: mmap/read the whole file, or loop `read` and tally `\n` without storing
everything. Correctness is the newline count, not the buffer strategy.

## Decimal buffer orientation

Build digits from the least significant end of a small stack/BSS buffer, then `write`
from the first digit. **Rejected wrong reading:** emit digits in reverse order to stdout
without reversing.

## Default path when argv[1] is missing

Use `TASK.md` (or whatever the task names). Open failure → non-zero exit.

## Distinctions worth keeping straight

- **Streaming count vs full buffer** — both valid; mmap is convenient, not mandatory.
- **Newline count vs `wc -w` words** — different problems.
- **Dynamic libc hello vs nostdlib tool** — NEEDED entries tell the story.
- **Digit conversion endianness of emission** — build reversed, then write forward.

## Deeper worked navigation (freestanding wc)

- Integrate argv+I/O+decimal without libc.
- Count newline bytes only.
- Prove no NEEDED libc.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| ./wc_nl file | decimal + newline |
| readelf -d | no libc NEEDED |
| reference | matches a known newline count |

### Ordered navigation moves

1. Parse path.
2. Count \n.
3. Emit decimal.
4. Verify freestanding.

### Rejected wrong readings (keep beside the artifact)

- printf for the count is fine.
- count words like wc -w.
- digits may print reversed.

### Tool-line decoding reminders

- decimal buffer reverse then write
- NEEDED search
- open failure → nonzero exit

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (46)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - printf for decimal
- counted words not newlines
- digits emitted reversed

    For each tick: write the *recognition* fix (which register/column/anchor) in one line.
    That diary is how this track sticks.

    ## Makefile → command → file (recap)

    | You type | Produces / runs | Open next |
    | -------- | --------------- | --------- |
    | `make` / `make bin` | exercise binary | run it; note exit status |
    | `make clean` | removes objects | before changing `O=` / flags |
    | `make asm` / `make disasm` (if any) | listing view | search the label you care about |
    | tools in Lookup | field dumps | decode columns, do not skim blobs |

    Remember: a disasm target usually *views* bytes already linked — it is not a new
    mysterious compile stage (lesson 01's `.lst` rule).

## Check yourself

1. How do you prove the binary has no libc dependency?
2. What is the output format required (digits + newline)?
3. Where does the pathname come from if argv[1] is absent?
4. Why is `printf("%d\n", n)` incompatible with the success criteria?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

## Key takeaways

- Syscall-only programs can still do useful I/O and formatting.
- Combining argv, file I/O, and integer rendering is the Track D integration test.
- Tooling (`nm`, `readelf -d`) verifies freestanding linkage.
- Correctness is a counted `\n` and an honest decimal print.

## Field-decoding recap for exercise 46

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 2 open`, `man 2 read`, `man 2 write`, `man 2 close`, `man 2 mmap`, `man 2 exit`
- `man 1 nm`, `man 1 readelf`, `unistd_64.h`

Now open `TASK.md` and do the practice.
