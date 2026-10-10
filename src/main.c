#include "ls.h"
#include <stdio.h>
#include <stdlib.h>

Options options = {0};
int exit_status = 0;

int main(int argc, char **argv)
{
    int first_operand = 1;

    parse_options(argc, argv, &first_operand);

    if (first_operand >= argc) {
        char *default_path[] = {"."};
        list_paths(1, default_path);
    } else {
        list_paths(argc - first_operand, argv + first_operand);
    }

    return exit_status;
}
