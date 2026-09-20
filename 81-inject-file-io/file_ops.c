#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_ops.h"

char *file_read_all(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }
    long n = ftell(f);
    if (n < 0) {
        fclose(f);
        return NULL;
    }
    rewind(f);
    char *buf = malloc((size_t)n + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    size_t got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    buf[got] = '\0';
    return buf;
}

static const char *g_fake_text;

void file_fake_set(const char *text)
{
    g_fake_text = text;
}

char *file_fake_read_all(const char *path)
{
    (void)path;
    if (g_fake_text == NULL) {
        return NULL;
    }
    return strdup(g_fake_text);
}

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
