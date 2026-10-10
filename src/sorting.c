#include "ls.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int compare_time(const FileEntry *a, const FileEntry *b)
{
    struct timespec ta, tb;

    if (options.time_type == 1) {
        ta = a->st.st_ctim;
        tb = b->st.st_ctim;
    } else if (options.time_type == 2) {
        ta = a->st.st_atim;
        tb = b->st.st_atim;
    } else {
        ta = a->st.st_mtim;
        tb = b->st.st_mtim;
    }

    if (ta.tv_sec > tb.tv_sec)
        return -1;
    if (ta.tv_sec < tb.tv_sec)
        return 1;

    if (ta.tv_nsec > tb.tv_nsec)
        return -1;
    if (ta.tv_nsec < tb.tv_nsec)
        return 1;

    return 0;
}

static int compare_entries(const void *left, const void *right)
{
    const FileEntry *a = left;
    const FileEntry *b = right;
    int result = 0;

    if (options.sort_size) {
        if (a->st.st_size > b->st.st_size)
            result = -1;
        else if (a->st.st_size < b->st.st_size)
            result = 1;
    } else if (options.sort_time) {
        result = compare_time(a, b);
    }

    if (result == 0)
        result = strcmp(a->name, b->name);

    return options.reverse ? -result : result;
}

void sort_entries(FileEntry *entries, size_t count)
{
    if (options.unsorted || count < 2)
        return;

    qsort(entries, count, sizeof(FileEntry), compare_entries);
}
