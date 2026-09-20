# 83 — Three binaries, one library graph
<!-- concepts: c.p4.build.cmake, c.p4.build.targets -->

Your RPG Makefile keeps three overlapping source lists: `SOURCES`,
`TEST_SOURCES`, and `FUZZ_SOURCES`. Forget `lib.c` (or `rng.c`) in one of
them and that binary silently links a different world. After this lesson
you can redraw that graph in CMake as one `add_library` plus three
`add_executable` calls — and say why the graph is the point, not the
generator syntax.

## What this lesson asks of you

Build with the legacy Makefile (three lists). Build again with the shipped
`CMakeLists.txt`. Diff the mental model: `lib.c` appears once. Fill
`student_CMakeLists.txt` as practice. This is content about CMake; the
repo still builds with plain `make` for every other exercise.

## The failure mode already true in ch24

Three lists, maintained by hand. Shared modules repeated. A missing entry
does not fail the build — it links whatever else you named, and tests the
wrong thing. That is the motivation. CMake (or a carefully factored
Makefile) is just how you make the dependency graph visible.

## Worked example

Legacy:

```make
SOURCES = main.c lib.c
TEST_SOURCES = test_combat.c lib.c
FUZZ_SOURCES = fuzz_parsers.c lib.c
```

CMake reference in this directory:

```cmake
add_library(gamelib STATIC lib.c)
add_executable(game main.c)
target_link_libraries(game PRIVATE gamelib)
# … run_tests and fuzz_parsers likewise
```

Configure and build:

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/run_tests
```

It is tempting to treat this exercise as "learn CMake syntax." That is
wrong, because the syntax is small; the claim is that **one node for
shared code** removes a class of silent test skew. If you copy three
lists into three `add_executable` source lines, you have reproduced the
bug in a new generator.

## Distinctions worth keeping straight

- **Target graph vs source list.** Edges versus copy-paste.
- **CMake as content vs CMake as this repo's build.** Taught here; not
  adopted for exercises 01–82.
- **Silent wrong link vs missing symbol.** The first is worse — it runs.

## Check yourself

1. What goes wrong if `TEST_SOURCES` forgets a shared module that still
   compiles via another path?
2. How many times should `lib.c` appear in a healthy graph?
3. Why does this repo keep `common.mk` for other exercises?
4. Name the three binaries in the miniature stand-in.

## Key takeaways

- Overlapping hand lists are a defect waiting to happen.
- One library target plus three executables makes sharing visible.
- CMake is the vehicle; the graph is the lesson.
- Your ch24 Makefile is the real-world instance of this miniature.

## Lookup (not the lesson)

- `cmake --help`, `cmake-commands(7)` — `add_library`, `add_executable`
- `ch24/Makefile` `SOURCES` / `TEST_SOURCES` / `FUZZ_SOURCES`

Now open `TASK.md` and do the practice.
