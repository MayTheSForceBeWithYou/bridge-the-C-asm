#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>
#include <string.h>

/* A whole test framework, in about twenty lines. It does exactly three
   things: count checks, report the ones that failed with a file and
   line, and set the exit status so `make test` fails the build. */

static int tests_run = 0;
static int tests_failed = 0;

#define CHECK(expr)                                                     \
    do {                                                                \
        tests_run++;                                                    \
        if (!(expr)) {                                                  \
            tests_failed++;                                             \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #expr);    \
        }                                                               \
    } while (0)

/* Widened to long long on purpose: several exercises are *about* width
   (long arguments in 04, long returns in 05, size_t byte counts in 82).
   Comparing those as int would truncate the very thing under test. */
#define CHECK_EQ(actual, expected)                                      \
    do {                                                                \
        tests_run++;                                                    \
        long long a_ = (long long)(actual);                             \
        long long e_ = (long long)(expected);                           \
        if (a_ != e_) {                                                 \
            tests_failed++;                                             \
            printf("  FAIL %s:%d: %s == %lld, expected %lld\n",         \
                   __FILE__, __LINE__, #actual, a_, e_);                \
        }                                                               \
    } while (0)

#define CHECK_STR(actual, expected)                                     \
    do {                                                                \
        tests_run++;                                                    \
        const char *a_ = (actual);                                      \
        const char *e_ = (expected);                                    \
        if (strcmp(a_, e_) != 0) {                                      \
            tests_failed++;                                             \
            printf("  FAIL %s:%d: %s == \"%s\", expected \"%s\"\n",     \
                   __FILE__, __LINE__, #actual, a_, e_);                \
        }                                                               \
    } while (0)

#define CHECK_PTR(actual, expected)                                     \
    do {                                                                \
        tests_run++;                                                    \
        const void *a_ = (actual);                                      \
        const void *e_ = (expected);                                    \
        if (a_ != e_) {                                                 \
            tests_failed++;                                             \
            printf("  FAIL %s:%d: %s == %p, expected %p\n",             \
                   __FILE__, __LINE__, #actual, a_, e_);                \
        }                                                               \
    } while (0)

static int test_report(void)
{
    printf("\n%d checks, %d failed\n", tests_run, tests_failed);
    return tests_failed != 0;
}

#endif
