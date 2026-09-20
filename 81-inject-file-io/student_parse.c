#include "file_ops.h"

/* Your version of parse.c. Build it instead of the reference with:
 *
 *     make clean && make SRCS="prog.c file_ops.c student_parse.c"
 *
 * Rules the reference follows and you should too: never call fopen, treat
 * a NULL read as -1, and free what read_all handed you. */

int parse_config_count(const struct FileOps *io, const char *path)
{
    /* TODO: read through io->read_all, count non-comment key=value lines,
       free the buffer, return the count (-1 if the read failed). */
    (void)io;
    (void)path;
    return -1;
}
