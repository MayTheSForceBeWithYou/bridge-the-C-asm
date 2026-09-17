# 47 — static vs dynamic hello (Track E)

## Goal
Same hello source; compare dynamic vs static (or static-pie) with `ldd`,
`size`, `readelf -d`.

## Do
1. Implement a tiny hello in `hello.c`.
2. Build dynamic (default) and attempt static:
```
make
cp hello hello.dyn
gcc -ggdb -static -o hello.static hello.c
# If -static fails on Arch (missing glibc static libs):
#   sudo pacman -S glibc  # static bits vary; or skip static and instead:
#   gcc -ggdb -static-pie -o hello.static hello.c
# If both fail: document the linker error and still deeply analyze hello.dyn
# with ldd / size / readelf -d / objdump -d.
```
3. Compare:
```
ldd ./hello.dyn
ldd ./hello.static || true
size ./hello.dyn ./hello.static
readelf -d ./hello.dyn
readelf -d ./hello.static || true
```
4. Answer: which has a dynamic section / INTERP? Which is larger? Why?

## Success
You have a filled comparison table (dyn always; static if link succeeded).

## Refs
`man 1 ldd`, `man 1 size`, `man 1 readelf`, `man 5 elf`, `man 1 gcc`
