#include <stdlib.h>
#include <string.h>

#include "file_ops.h"

/* Reference implementation of the code under test — the translation unit
   student_parse.c replaces. It never calls fopen: bytes arrive through
   io->read_all, and the same free() serves both leaves. */

int parse_config_count(const struct FileOps *io, const char *path)
{
    char *text = io->read_all(path);
    if (text == NULL) {
        return -1;
    }

    int count = 0;
    char *save = NULL;
    for (char *line = strtok_r(text, "\n", &save); line != NULL;
         line = strtok_r(NULL, "\n", &save)) {
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        if (strchr(line, '=') != NULL) {
            count++;
        }
    }

    free(text);
    return count;
}
