#include "ls.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int visible(const char *name)
{
    if (options.all)
        return 1;

    if (options.almost_all)
        return strcmp(name, ".") != 0 &&
               strcmp(name, "..") != 0;

    return name[0] != '.';
}

static void add_entry(FileEntry **entries, size_t *count,
                      size_t *capacity, const char *path,
                      const char *name, const struct stat *st)
{
    FileEntry *entry;

    if (*count == *capacity) {
        size_t new_capacity = *capacity ? *capacity * 2 : 32;
        FileEntry *new_entries = realloc(
            *entries, new_capacity * sizeof(FileEntry));

        if (new_entries == NULL) {
            perror("myls: realloc");
            exit(EXIT_FAILURE);
        }

        *entries = new_entries;
        *capacity = new_capacity;
    }

    entry = &(*entries)[(*count)++];
    entry->name = duplicate_string(name);
    entry->path = duplicate_string(path);
    entry->st = *st;
    entry->stat_ok = 1;
}

static void show_entries(FileEntry *entries, size_t count)
{
    size_t i;

    sort_entries(entries, count);

    calculate_widths(entries, count);
    for (i = 0; i < count; ++i)
        print_entry(&entries[i]);
}

void list_directory(const char *path, int show_header)
{
    DIR *dir;
    struct dirent *de;
    FileEntry *entries = NULL;
    size_t count = 0, capacity = 0, i;

    dir = opendir(path);
    if (dir == NULL) {
        report_error(path);
        return;
    }

    if (show_header)
        printf("%s:\n", path);

    while ((de = readdir(dir)) != NULL) {
        struct stat st;
        char *full;

        if (!visible(de->d_name))
            continue;

        full = join_path(path, de->d_name);

        if (lstat(full, &st) != 0) {
            report_error(full);
            free(full);
            continue;
        }

        add_entry(&entries, &count, &capacity,
                  full, de->d_name, &st);
        free(full);
    }

    closedir(dir);

    sort_entries(entries, count);
    calculate_widths(entries, count);
    if (options.long_format) print_total(entries, count);

    for (i = 0; i < count; ++i)
        print_entry(&entries[i]);

    if (options.recursive) {
        for (i = 0; i < count; ++i) {
            if (!S_ISDIR(entries[i].st.st_mode))
                continue;

            if (strcmp(entries[i].name, ".") == 0 ||
                strcmp(entries[i].name, "..") == 0)
                continue;

            putchar('\n');
            list_directory(entries[i].path, 1);
        }
    }

    free_entries(entries, count);
}

void list_paths(int count, char **paths)
{
    FileEntry *files = NULL;
    FileEntry *dirs = NULL;
    size_t file_count = 0, dir_count = 0;
    size_t file_capacity = 0, dir_capacity = 0;
    int i;
    int printed_files = 0;

    for (i = 0; i < count; ++i) {
        struct stat st;

        if (lstat(paths[i], &st) != 0) {
            report_error(paths[i]);
            continue;
        }

        if (S_ISDIR(st.st_mode) && !options.directory) {
            add_entry(&dirs, &dir_count, &dir_capacity,
                      paths[i], paths[i], &st);
        } else {
            add_entry(&files, &file_count, &file_capacity,
                      paths[i], paths[i], &st);
        }
    }

    show_entries(files, file_count);
    printed_files = file_count > 0;

    sort_entries(dirs, dir_count);

    for (i = 0; i < (int)dir_count; ++i) {
        if (printed_files || i > 0)
            putchar('\n');

        list_directory(dirs[i].path,
                       printed_files || dir_count > 1);
    }

    free_entries(files, file_count);
    free_entries(dirs, dir_count);
}
