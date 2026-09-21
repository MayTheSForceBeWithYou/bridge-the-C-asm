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

## Distinctions worth keeping straight

- **Logical value vs memory byte order** — both matter; they are not the same view.
- **Host LE vs protocol BE** — bswap bridges them.
- **Builtin vs portable shifts** — builtin maps cleanly to `bswap` here.
- **Char walk signedness** — use `unsigned char` to avoid sign-extension surprises when
  printing.

## Check yourself

1. What byte sits at the lowest address for `0x12345678` on this machine?
2. What integer value does `__builtin_bswap32(0x12345678)` produce?
3. Where does endianness show up — in pure register arithmetic or in memory layout?
4. What mnemonic should `-O2` asm show for the builtin?

## Key takeaways

- x86-64 userland here is little-endian.
- Byte walks reveal storage order; `%x` of the word reveals the value.
- `bswap` flips bytes for endian conversion.
- Builtins make the hardware instruction accessible from C.

## Lookup (not the lesson)

- `man 1 gcc`, `man 1 objdump`, `info gcc`

Now open `TASK.md` and do the practice.
