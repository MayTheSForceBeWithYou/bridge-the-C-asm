# 67 — Little-endian bytes and bswap

x86-64 is little-endian: the least significant byte of an integer sits at the lowest
address. Walking a `uint32_t` via `unsigned char *` shows that order. Byte-swapping
(for big-endian protocols or file formats) reverses the bytes; GCC's
`__builtin_bswap32` typically becomes a single `bswap` instruction at `-O2`. This lesson
makes both visible.

## What this lesson asks of you

Set `x = 0x12345678u`, print bytes via a `char*`, compute `y = __builtin_bswap32(x)`,
print bytes again, confirm LE expectation and swapped value, and find `bswap` in
`-O2` asm. Print `ok`.

## Little-endian picture

For `0x12345678` on LE:

| Address offset | Byte |
| -------------- | ---- |
| 0 | `0x78` |
| 1 | `0x56` |
| 2 | `0x34` |
| 3 | `0x12` |

After bswap, the integer value is `0x78563412`, and memory bytes reverse accordingly.

## Little-endian picture for `0x12345678`

| Address offset | Byte |
| -------------- | ---- |
| 0 | `0x78` |
| 1 | `0x56` |
| 2 | `0x34` |
| 3 | `0x12` |

After `__builtin_bswap32`, value is `0x78563412`.

## Mapping `make` to checks

| You type | Check afterward |
| -------- | --------------- |
| `make` | byte walk + swapped value; `ok` |
| `make O=2 asm` | `bswap` mnemonic in your function |

## Logical value vs memory order

Printing `%x` of the `uint32_t` shows the logical value. The `unsigned char *` walk shows
address order. **Rejected wrong reading:** `%x` already shows memory order.

## Endianness appears at serialization

Register-only arithmetic does not "have endianness" in the same way. Endianness shows
when you lay bytes in memory or on the wire.

**Navigation:** cast to `unsigned char *` (not plain `char *`) to avoid sign-extension
surprises when printing bytes.

## Worked example

**The situation.** First byte printed is `78`; after swap, value matches expectation;
asm contains `bswap`; `ok`.

**Step 1 — walk bytes.** Cast to `unsigned char *`. The rejected wrong reading is:
"printing `%x` of the uint32_t already shows memory order." Printing the word shows the
logical value; the char walk shows address order.

**Step 2 — bswap.** Builtin or manual shifts. The rejected wrong reading is: "endianness
changes when I assign between registers." Register values are just integers; endianness
appears when you *serialize* to memory or the wire.

**Step 3 — asm.** `-O2` should show `bswap` for the builtin.

## Worked byte walk

```c
uint32_t x = 0x12345678u;
unsigned char *p = (unsigned char *)&x;
/* expect p[0]==0x78 on this LE machine */
uint32_t y = __builtin_bswap32(x); /* 0x78563412 */
```

At `-O2`, search your function for `bswap`. **Rejected wrong reading:** assigning between
registers changes endianness — endianness appears when serializing to memory/wire.

## Distinctions worth keeping straight

- **Logical value vs memory byte order** — both matter; they are not the same view.
- **Host LE vs protocol BE** — bswap bridges them.
- **Builtin vs portable shifts** — builtin maps cleanly to `bswap` here.
- **Char walk signedness** — use `unsigned char` to avoid sign-extension surprises when
  printing.

## Deeper worked navigation (endian bswap)

- x86-64 LE: 0x12345678 → bytes 78 56 34 12.
- bswap flips for BE protocols.
- %x is logical value; char walk is memory order.

### Artifact map

| Artifact / command | What you should notice |
| ------------------ | ---------------------- |
| byte walk | 78 first |
| bswap value | 0x78563412 |
| O2 asm | bswap mnemonic |

### Ordered navigation moves

1. Walk bytes.
2. bswap.
3. Find asm.

### Rejected wrong readings (keep beside the artifact)

- %x shows memory order.
- register assign changes endianness.
- signed char walk is fine.

### Tool-line decoding reminders

- LE table
- builtin→bswap
- unsigned char

### Self-check micro-drill

Close the listing and answer: (a) what did you search for first, (b) which column/field
was load-bearing, (c) which wrong reading did you almost make? Re-open only to verify.

    ## Common failure diary (67)

    After you finish the lab, tick any you actually hit (honest notes beat pride):

    - %x shows memory order
- signed char walk

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

1. What byte sits at the lowest address for `0x12345678` on this machine?
2. What integer value does `__builtin_bswap32(0x12345678)` produce?
3. Where does endianness show up — in pure register arithmetic or in memory layout?
4. What mnemonic should `-O2` asm show for the builtin?

5. What exact search/anchor takes you to the load-bearing artifact in this exercise?
6. On one multi-field tool line you used, which token is which kind of information?
7. Name one rejected wrong reading for this lesson's central artifact.

If any answer is fuzzy, re-read the matching section above — do not open man pages yet.
When you need a flag spelling, *then* use Lookup.

8. Which Makefile target (if any) only *views* bytes already built, without a new compile stage?
9. What is one optional tool in this lesson, and what do you do if it is missing?

Anchor drill: write the exact search string you used in the primary listing, then
quote one real line from your machine with every field labeled. If you cannot label
a field, that field is still unlearned — return to the decoding table above.

## Key takeaways

- x86-64 userland here is little-endian.
- Byte walks reveal storage order; `%x` of the word reveals the value.
- `bswap` flips bytes for endian conversion.
- Builtins make the hardware instruction accessible from C.

## Field-decoding recap for exercise 67

Re-state the recognition rules in your own notes after the lab:

1. Name the anchor search string you used in the main artifact.
2. Copy one real tool line from your machine and label each column/field.
3. Write the rejected wrong reading you personally almost made.
4. Map each Makefile target you invoked to a file you opened.

This recap is part of the competence — unlabelled hex dumps do not count.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, `info gcc`

Now open `TASK.md` and do the practice.
