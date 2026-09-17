# Verified local references

Only point students at these. If it is not listed, do not invent a man page —
discover from compiler/tool output instead.

## nvim-dap (this machine)

In-editor debugging via **CodeLLDB** (mason-installed) + nvim-dap / dap-ui /
dap-virtual-text. Cheat sheet: **`NVIM_DAP.md`**. Project launch configs:
`.vscode/launch.json` (loaded by `dap.ext.vscode.load_launchjs`).

- Keys, multi-file workflow, memory view, and gdb↔dap mapping: `NVIM_DAP.md`
- Neovim help: `:help dap.txt` (when the plugin is installed)
- Plugin docs: Lazy UI / upstream nvim-dap, nvim-dap-ui, nvim-dap-virtual-text
- **No** fake man pages for dap / CodeLLDB

Track C (32–39) and dap drills (69–71) can be done in **nvim-dap** and/or
**gdb CLI** — same skills, different UI.

## Manual pages (section 1 — tools)

| Command | Why |
|---------|-----|
| `man 1 gcc` | Driver flags: `-E`, `-S`, `-c`, `-O`, `-fverbose-asm`, `-nostdlib`, linking |
| `man 1 as` | GNU assembler (AT&T); directives, syntax |
| `man 1 ld` | Linker; freestanding `_start` programs |
| `man 1 objdump` | Disassemble (`-d`), headers (`-h`), Intel syntax (`-M intel`), `-j .plt` |
| `man 1 nm` | Symbol table: T/D/B/R … where things landed |
| `man 1 size` | Text/data/bss sizes |
| `man 1 readelf` | ELF structure deep dive |
| `man 1 gdb` | Debugger entry; then use `gdb` interactive help |
| `man 1 strip` | What disappears when symbols are stripped |
| `man 1 addr2line` | Address → source line (with `-g`) |
| `man 1 ldd` | Dynamic linker dependency walk |

## Manual pages (section 2 — syscalls)

| Command | Why |
|---------|-----|
| `man 2 syscall` | Raw syscall wrapper / convention overview |
| `man 2 syscalls` | Catalog of Linux syscalls |
| `man 2 read` | `read(2)` |
| `man 2 write` | `write(2)` — useful contrast to raw `__NR_write` |
| `man 2 open` | File open syscall docs |
| `man 2 close` | Close fd |
| `man 2 exit` | Process termination (libc-facing; compare to `__NR_exit`) |
| `man 2 brk` | Classic heap break |
| `man 2 sbrk` | `sbrk` wrapper around the break |
| `man 2 mmap` | Memory mapping |
| `man 2 munmap` | Unmap |
| `man 2 sigaction` | Install signal handlers |
| `man 2 getpid` | Pid (handy in handlers / debugging) |

## Manual pages (section 3 — libc)

| Command | Why |
|---------|-----|
| `man 3 printf` | Formatted output used by many harnesses |
| `man 3 exit` | `exit(3)` vs process exit |
| `man 3 pthread_create` | Threads (false-sharing exercise) |
| `man 3 memcpy` | Byte copy; standard type-pun alternative to unions |

## Manual pages (section 5 / 7)

| Command | Why |
|---------|-----|
| `man 5 elf` | ELF object/executable format |
| `man 7 signal` | Signals (handlers, crash exercises) |

## GNU info manuals

| Command | Why |
|---------|-----|
| `info as` | Assembler manual (directives, expressions) |
| `info binutils` | Binutils overview; jump to `objdump`/`nm`/`ld` nodes |
| `info gcc` | GCC manual — optimization, Extended Asm, visibility |
| `info gdb` | Debugger manual (incl. TUI) |
| `info ld` | Linker scripts / options in depth |
| `info libc` | libc internals / startup notes when useful (no man for `__libc_start_main`) |

## Headers / local binaries

| Path / command | Why |
|----------------|-----|
| `/usr/include/asm/unistd_64.h` | `__NR_read` 0, `__NR_write` 1, `__NR_open` 2, `__NR_close` 3, `__NR_mmap` 9, `__NR_munmap` 11, `__NR_brk` 12, `__NR_exit` 60, `__NR_openat` 257, … |
| `ausyscall` | Name ↔ number lookup for syscalls |
| `grep __NR_ /usr/include/asm/unistd_64.h` | Find syscall numbers without memorizing |

## gdb interactive help (no separate man needed)

Inside `gdb`:

- `help disassemble`
- `help stepi` / `help nexti`
- `help info registers`
- `help watch` / `help x`
- `help layout` (TUI)

## Useful one-liners (flags verified available)

```bash
gcc -S -fverbose-asm -O0 foo.c -o foo.s
gcc -S -fverbose-asm -O2 foo.c -o foo.s
gcc -O3 -march=native -fopt-info-vec-optimized ...
gcc -no-pie ...          # Type: EXEC (default PIE → DYN)
objdump -d foo
objdump -d -j .plt foo
objdump -d -M intel foo
objdump -h foo.o
nm foo
size foo
readelf -a foo
readelf -d foo
readelf --dyn-syms foo
ldd foo
strip foo.copy
addr2line -e foo 0xADDR
```

## Hardware facts (this machine)

- L1D cache line: **64** bytes (`getconf LEVEL1_DCACHE_LINESIZE`)
- Use **`rdtscp`** (not plain `rdtsc`); `constant_tsc` present
- `-lpthread` links cleanly

## Optional tools (not installed by default)

- `strace` — `command -v strace` or `sudo pacman -S strace`; always have a no-strace fallback
- `perf` — `command -v perf` or `sudo pacman -S perf`; always have rdtscp fallback
- Do **not** cite `man strace` / `man perf` as required reading

## Explicitly NOT available here — do not cite

- `man 2 _exit`
- `man 3 syscall`
- `man 3 __libc_start_main` (use `info libc` or disassembly)
- Any System V ABI PDF / man page
- nasm documentation

For ABI: read what `gcc -S` and `objdump` emit. For raw syscalls: `man 2 syscall` +
`man 2 syscalls` + `unistd_64.h` / `ausyscall`.
