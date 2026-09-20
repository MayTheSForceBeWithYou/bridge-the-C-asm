#ifndef FILE_OPS_H
#define FILE_OPS_H

struct FileOps {
    char *(*read_all)(const char *path);
};

char *file_read_all(const char *path);
char *file_fake_read_all(const char *path);
void file_fake_set(const char *text);

/* Count non-comment key=value lines. Caller owns nothing. */
int parse_config_count(const struct FileOps *io, const char *path);

#endif
