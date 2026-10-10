#include "ls.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#if defined(__linux__)
#include <sys/sysmacros.h>
#endif

static unsigned long long block_count(const struct stat *st)
{
    unsigned long long bytes =
        (unsigned long long)st->st_blocks * 512ULL;
    unsigned long long unit = 512ULL;
    const char *env = getenv("BLOCKSIZE");

    if (options.kilobytes)
        unit = 1024ULL;
    else if (env != NULL && *env != '\0') {
        char *end;
        unsigned long long value = strtoull(env, &end, 10);

        if (value > 0) {
            if (*end == 'K' || *end == 'k') {
                value *= 1024ULL;
                ++end;
            } else if (*end == 'M' || *end == 'm') {
                value *= 1024ULL * 1024ULL;
                ++end;
            }

            if (*end == '\0')
                unit = value;
        }
    }

    return (bytes + unit - 1) / unit;
}

static void human_size(unsigned long long bytes,
                       char *buffer, size_t length)
{
    static const char *units[] = {"B", "K", "M", "G", "T", "P"};
    double size = (double)bytes;
    size_t i = 0;

    while (size >= 1024.0 && i < 5) {
        size /= 1024.0;
        ++i;
    }

    if (i == 0)
        snprintf(buffer, length, "%lluB", bytes);
    else if (size < 10.0)
        snprintf(buffer, length, "%.1f%s", size, units[i]);
    else
        snprintf(buffer, length, "%.0f%s", size, units[i]);
}

static void print_name(const char *name)
{
    const unsigned char *p = (const unsigned char *)name;

    while (*p != '\0') {
        if (options.quote && !isprint(*p))
            putchar('?');
        else
            putchar(*p);
        ++p;
    }
}

static char classify_char(mode_t mode)
{
    if (S_ISDIR(mode))
        return '/';
    if (S_ISLNK(mode))
        return '@';
    if (S_ISFIFO(mode))
        return '|';
    if (S_ISSOCK(mode))
        return '=';
    if (S_ISWHT(mode))
        return '%';
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        return '*';
    return '\0';
}

static void print_mode(mode_t mode)
{
    char s[11] = "----------";

    if (S_ISDIR(mode))
        s[0] = 'd';
    else if (S_ISLNK(mode))
        s[0] = 'l';
    else if (S_ISCHR(mode))
        s[0] = 'c';
    else if (S_ISBLK(mode))
        s[0] = 'b';
    else if (S_ISFIFO(mode))
        s[0] = 'p';
    else if (S_ISSOCK(mode))
        s[0] = 's';

    if (mode & S_IRUSR) s[1] = 'r';
    if (mode & S_IWUSR) s[2] = 'w';
    if (mode & S_IXUSR) s[3] = 'x';
    if (mode & S_IRGRP) s[4] = 'r';
    if (mode & S_IWGRP) s[5] = 'w';
    if (mode & S_IXGRP) s[6] = 'x';
    if (mode & S_IROTH) s[7] = 'r';
    if (mode & S_IWOTH) s[8] = 'w';
    if (mode & S_IXOTH) s[9] = 'x';

    if (mode & S_ISUID)
        s[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        s[6] = (mode & S_IXGRP) ? 's' : 'S';
    if (mode & S_ISVTX)
        s[9] = (mode & S_IXOTH) ? 't' : 'T';

    printf("%s", s);
}

static ColumnWidths widths;
static void print_owner(uid_t uid)
{
    struct passwd *pw = options.numeric ? NULL : getpwuid(uid);
    char buffer[64];
    const char *name;

    if (pw != NULL)
        name = pw->pw_name;
    else {
        snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)uid);
        name = buffer;
    }
    printf("%-*s", widths.owner, name);
}

static void print_group(gid_t gid)
{
    struct group *gr = options.numeric ? NULL : getgrgid(gid);
    char buffer[64];
    const char *name;

    if (gr != NULL)
        name = gr->gr_name;
    else {
        snprintf(buffer, sizeof(buffer), "%lu", (unsigned long)gid);
        name = buffer;
    }
    printf("%-*s", widths.group, name);
}

static void print_timestamp(const struct stat *st)
{
    time_t value;
    time_t now = time(NULL);
    struct tm *tm;
    char buffer[64];

    if (options.time_type == 1)
        value = st->st_ctime;
    else if (options.time_type == 2)
        value = st->st_atime;
    else
        value = st->st_mtime;

    tm = localtime(&value);
    if (tm == NULL) {
        printf("??? ?? ??:?? ");
        return;
    }

    if (value > now || now - value > 15552000)
        strftime(buffer, sizeof(buffer), "%b %e  %Y", tm);
    else
        strftime(buffer, sizeof(buffer), "%b %e %H:%M", tm);

            printf("%*s ",
                   options.long_format ? widths.size : widths.blocks,
                   buffer);
}

void print_entry(const FileEntry *entry)
{
    const struct stat *st = &entry->st;
    char buffer[64];

    if (options.inode)
        printf("%*llu ", widths.inode,
               (unsigned long long)st->st_ino);

    if (options.blocks) {
        if (options.human) {
            human_size(
                (unsigned long long)st->st_blocks * 512ULL,
                buffer, sizeof(buffer));
            printf("%*s ",
                   widths.blocks,
                   buffer);
        } else {
            printf("%*llu ", widths.blocks, block_count(st));
        }
    }

    if (options.long_format) {
        print_mode(st->st_mode);
        printf("  %*lu ", widths.links,
               (unsigned long)st->st_nlink);

        print_owner(st->st_uid);
        printf("  ");
        print_group(st->st_gid);
        printf("  ");

        if (S_ISCHR(st->st_mode) || S_ISBLK(st->st_mode)) {
            printf("%u, %u ",
                   (unsigned int)major(st->st_rdev),
                   (unsigned int)minor(st->st_rdev));
        } else if (options.human) {
            human_size((unsigned long long)st->st_size,
                       buffer, sizeof(buffer));
            printf("%*s ",
                   widths.size,
                   buffer);
        } else {
            printf("%*lld ", widths.size,
                   (long long)st->st_size);
        }

        print_timestamp(st);
    }

    print_name(entry->name);

    if (options.classify) {
        char indicator = classify_char(st->st_mode);
        if (indicator != '\0')
            putchar(indicator);
    }

    if (options.long_format && S_ISLNK(st->st_mode)) {
        char target[4096];
        ssize_t length = readlink(entry->path, target,
                                  sizeof(target) - 1);

        if (length >= 0) {
            target[length] = '\0';
            printf(" -> ");
            print_name(target);
        }
    }

    putchar('\n');
}

void print_total(const FileEntry *entries, size_t count)
{
    size_t i;
    unsigned long long total = 0;
    char buffer[64];

    if (!options.long_format && !options.blocks)
        return;

    for (i = 0; i < count; ++i)
        total += (unsigned long long)entries[i].st.st_blocks;

    if (options.human) {
        unsigned long long size_total = 0;
        for (i = 0; i < count; ++i)
            size_total += (unsigned long long)entries[i].st.st_size;
        human_size(size_total, buffer, sizeof(buffer));
        printf("total %s\n", buffer);
    } else {
        struct stat temp;
        memset(&temp, 0, sizeof(temp));
        temp.st_blocks = (blkcnt_t)total;
        printf("total %llu\n", block_count(&temp));
    }
}


void calculate_widths(const FileEntry *entries, size_t count)
{
    size_t i;
    char buffer[128];
    struct passwd *pw;
    struct group *gr;
    int n;

    memset(&widths, 0, sizeof(widths));

    if (options.long_format && options.human)
        widths.size = 4;

    for (i = 0; i < count; ++i) {
        const struct stat *st = &entries[i].st;

        n = snprintf(buffer, sizeof(buffer), "%lu",
                     (unsigned long)st->st_nlink);
        if (n > widths.links)
            widths.links = n;

        pw = options.numeric ? NULL : getpwuid(st->st_uid);
        if (pw != NULL)
            n = (int)strlen(pw->pw_name);
        else
            n = snprintf(buffer, sizeof(buffer), "%lu",
                         (unsigned long)st->st_uid);
        if (n > widths.owner)
            widths.owner = n;

        gr = options.numeric ? NULL : getgrgid(st->st_gid);
        if (gr != NULL)
            n = (int)strlen(gr->gr_name);
        else
            n = snprintf(buffer, sizeof(buffer), "%lu",
                         (unsigned long)st->st_gid);
        if (n > widths.group)
            widths.group = n;

        if (options.human) {
            human_size((unsigned long long)st->st_size,
                       buffer, sizeof(buffer));
            n = (int)strlen(buffer);
        } else {
            n = snprintf(buffer, sizeof(buffer), "%lld",
                         (long long)st->st_size);
        }
        if (n > widths.size)
            widths.size = n;

        n = snprintf(buffer, sizeof(buffer), "%llu",
                     (unsigned long long)st->st_ino);
        if (n > widths.inode)
            widths.inode = n;

        if (options.human) {
            human_size((unsigned long long)st->st_blocks * 512ULL,
                       buffer, sizeof(buffer));
            n = (int)strlen(buffer);
        } else {
            n = snprintf(buffer, sizeof(buffer), "%llu",
                         block_count(st));
        }
        if (n > widths.blocks)
            widths.blocks = n;
    }
}
