#ifndef FILE_OPS_H
#define FILE_OPS_H

/* The seam. Both leaves return a heap buffer the caller must free, or NULL
   on failure — one ownership rule, so the parser cannot tell them apart. */
struct FileOps {
    char *(*read_all)(const char *path);
};

char *file_read_all(const char *path);       /* real: opens and slurps */
char *file_fake_read_all(const char *path);  /* fake: strdups planted text */
void file_fake_set(const char *text);        /* NULL plants a read failure */

/* Counts non-comment key=value lines; -1 if the read failed. Frees the
   buffer read_all returned, so the caller owns nothing afterwards. */
int parse_config_count(const struct FileOps *io, const char *path);

#endif
