#include <stdio.h>

#include "src/argparse.h"
#include "src/utils.h"
#include "src/ls.h"

#define MAX_POSARGS 1024

int main(int argc, char* argv[]) {
    int posarg_idices[MAX_POSARGS];
    minils_opt_flag_t opt_flag = 0;

    if (minils_parse_args(argc, argv, &opt_flag, posarg_idices,
                          sizeof(posarg_idices))) {
        fprintf(stderr, "Failed to parse arguments\n");
        return -1;
    }

    int num_posargs =
        minils_get_posargs_len(posarg_idices, sizeof(posarg_idices));

    char* posargs[MAX_POSARGS];

    for (int i = 0; i < num_posargs; ++i) {
        posargs[i] = argv[posarg_idices[i]];
    }

    minils_ls(posargs, num_posargs, opt_flag);

    return 0;
}