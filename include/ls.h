#ifndef MYLS_H
#define MYLS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>

typedef struct {
    int all;
    int almost_all;
    int directory;
    int classify;
    int unsorted;
    int human;
    int inode;
    int kilobytes;
    int long_format;
    int numeric;
    int quote;
    int recursive;
    int reverse;
    int sort_size;
    int blocks;
    int sort_time;
    int time_type;
} Options;

typedef struct {
    char *name;
    char *path;
    struct stat st;
    int stat_ok;
} FileEntry;

extern Options options;
extern int exit_status;

void parse_options(int argc, char **argv, int *first_operand);

void list_paths(int count, char **paths);
void list_directory(const char *path, int show_header);

void sort_entries(FileEntry *entries, size_t count);

typedef struct {
    int links;
    int owner;
    int group;
    int size;
    int inode;
    int blocks;
} ColumnWidths;

void calculate_widths(const FileEntry *entries, size_t count);

void print_entry(const FileEntry *entry);
void print_total(const FileEntry *entries, size_t count);

char *join_path(const char *dir, const char *name);
char *duplicate_string(const char *text);
void report_error(const char *path);
void free_entries(FileEntry *entries, size_t count);

#endif
