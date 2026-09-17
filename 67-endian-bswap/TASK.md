# 67 — endianness / bswap (Track G)

## Goal
Walk a multi-byte constant via `char*` on little-endian x86-64; flip with
`__builtin_bswap32` / observe `bswap` mnemonic.

## Do
1. Set `uint32_t x = 0x12345678u`; print bytes via `(unsigned char *)&x`.
2. `y = __builtin_bswap32(x)`; print bytes again.
3. `make O=2 asm` — find `bswap`.

## Success
Byte order matches LE expectation; swapped value correct. `ok`.

## Refs
`man 1 gcc`, `man 1 objdump`, `info gcc`
