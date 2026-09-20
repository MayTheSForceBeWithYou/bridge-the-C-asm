cmake_minimum_required(VERSION 3.16)
project(three_binaries C)

# One node for the shared code. There is no list to keep in sync, so the
# skew the Makefile above ships cannot be expressed by accident: combat.c
# is named once, and combat_legacy.c is named nowhere.
add_library(gamelib STATIC combat.c config.c rng.c)
target_include_directories(gamelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(game main.c)
target_link_libraries(game PRIVATE gamelib)

add_executable(run_tests test_combat.c)
target_link_libraries(run_tests PRIVATE gamelib)

add_executable(fuzz_parsers fuzz_parsers.c)
target_link_libraries(fuzz_parsers PRIVATE gamelib)
