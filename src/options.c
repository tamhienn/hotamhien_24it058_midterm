#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void parse_options(int argc, char **argv, int *first_operand)
{
    int ch;

    while ((ch = getopt(argc, argv,
                        "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
        case 'A':
            options.almost_all = 1;
            break;
        case 'a':
            options.all = 1;
            break;
        case 'c':
            options.time_type = 1;
            break;
        case 'd':
            options.directory = 1;
            options.recursive = 0;
            break;
        case 'F':
            options.classify = 1;
            break;
        case 'f':
            options.unsorted = 1;
            break;
        case 'h':
            options.human = 1;
            options.kilobytes = 0;
            break;
        case 'i':
            options.inode = 1;
            break;
        case 'k':
            options.kilobytes = 1;
            options.human = 0;
            break;
        case 'l':
            options.long_format = 1;
            options.numeric = 0;
            break;
        case 'n':
            options.long_format = 1;
            options.numeric = 1;
            break;
        case 'q':
            options.quote = 1;
            break;
        case 'R':
            options.recursive = 1;
            options.directory = 0;
            break;
        case 'r':
            options.reverse = 1;
            break;
        case 'S':
            options.sort_size = 1;
            options.sort_time = 0;
            break;
        case 's':
            options.blocks = 1;
            break;
        case 't':
            options.sort_time = 1;
            options.sort_size = 0;
            break;
        case 'u':
            options.time_type = 2;
            break;
        case 'w':
            options.quote = 0;
            break;
        default:
            fprintf(stderr,
                "usage: myls [-AacdFfhiklnqRrSstuw] [file ...]\n");
            exit(EXIT_FAILURE);
        }
    }

    *first_operand = optind;
}
