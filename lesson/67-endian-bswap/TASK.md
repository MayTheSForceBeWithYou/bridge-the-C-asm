# 67 — endianness / bswap (practice)

Read `LESSON.md` first. This file is only the lab.

## Implement

1. `uint32_t x = 0x12345678u`; print bytes via `(unsigned char *)&x`.
2. `y = __builtin_bswap32(x)`; print bytes again.
3. `make O=2 asm` — find `bswap`. Confirm LE + swapped value; `ok`.

## Done when

- Byte order matches LE; swapped value correct; `ok`.

## Lookup

`man 1 gcc`, `man 1 objdump`, `info gcc`.
