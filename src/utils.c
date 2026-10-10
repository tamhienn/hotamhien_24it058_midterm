#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *duplicate_string(const char *text)
{
    char *copy = strdup(text);
    if (copy == NULL) {
        perror("myls: strdup");
        exit(EXIT_FAILURE);
    }
    return copy;
}

char *join_path(const char *dir, const char *name)
{
    size_t a = strlen(dir);
    size_t b = strlen(name);
    int slash = (a > 0 && dir[a - 1] != '/');
    char *result = malloc(a + b + slash + 1);

    if (result == NULL) {
        perror("myls: malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(result, dir, a);
    if (slash)
        result[a++] = '/';
    memcpy(result + a, name, b + 1);

    return result;
}

void report_error(const char *path)
{
    fprintf(stderr, "myls: ");
    perror(path);
    exit_status = 1;
}

void free_entries(FileEntry *entries, size_t count)
{
    size_t i;

    for (i = 0; i < count; ++i) {
        free(entries[i].name);
        free(entries[i].path);
    }

    free(entries);
}
