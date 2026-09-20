cmake_minimum_required(VERSION 3.16)
project(three_binaries C)

# One library, three executables — lib.c appears once.
add_library(gamelib STATIC lib.c)

add_executable(game main.c)
target_link_libraries(game PRIVATE gamelib)

add_executable(run_tests test_combat.c)
target_link_libraries(run_tests PRIVATE gamelib)

add_executable(fuzz_parsers fuzz_parsers.c)
target_link_libraries(fuzz_parsers PRIVATE gamelib)
