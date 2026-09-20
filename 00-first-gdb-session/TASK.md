# 00 — A first gdb session (practice)

Read `LESSON.md` first. This file is only the lab.

## Do

1. `make` in this directory. Run `./count` once and note the printed total.
2. Start gdb the quiet way:

   ```bash
   gdb -q -iex 'set debuginfod enabled off' ./count
   ```

3. `break main`, `run`, `info locals`. Then `next` until you are on
   `total += i` for the first time. `print i` and `print total`.
4. Take one more `next`. `print total` again. Confirm it moved.
5. Set `break 9` (the add line). `continue` a few times, printing `i` and
   `total` at each stop, until you have watched at least three iterations.
6. `continue` until the process exits. `quit`.

Stay inside these commands only: `break`, `run`, `next`, `step`, `print`,
`info locals`, `continue`, `quit`. No `x/`, no `disassemble`, no registers.

## Done when

- You saw `total` climb across loop iterations under gdb, not only as the final
  `total=15` on stdout.
- You can say in one sentence what `next` did for you that `continue` did not.
- `./count` still prints `total=15` when run outside gdb.

## Lookup

`man 1 gdb`, gdb `help next`, `help print`.
