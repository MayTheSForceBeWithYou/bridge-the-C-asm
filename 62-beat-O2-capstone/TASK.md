# 62 — beat -O2 capstone (practice)

Read `LESSON.md` first. This file is only the lab.

## Compete

1. Read `naive_sum`; measure at `-O2` — do not "optimize the compiler."
2. Write `fast_sum` with a better approach (branchless, blocked, etc.).
3. `make O=2 && ./prog` — harness fails unless fast beats naive (warmup OK). Re-run.

## Done when

- Both cycle counts printed; `ok` when fast wins consistently.

## Lookup

exercises 55–60, 31; `man 1 gcc`.
