#include "src/argparse.h"

#include <unistd.h>

#include <stdio.h>

int minils_parse_args(int argc, char* const* argv, minils_opt_flag_t* opt_flag,
                      int* posarg_idices, int max_pos_args) {
    int ch;
    int pos_arg_idx = 0;

    while ((ch = getopt(argc, argv, "-liRa")) != -1) {
        switch (ch) {
            case 1:  // positional arg
                if (pos_arg_idx < max_pos_args - 1) {
                    posarg_idices[pos_arg_idx++] = optind - 1;
                } else {
                    fprintf(stderr,
                            "number of positional args exceeded max_pos_args - "
                            "1\n");
                    return -1;
                }
                break;
            case 'a':
                *opt_flag |= MINILS_OPT_A_FLAG;
                break;
            case 'l':
                *opt_flag |= MINILS_OPT_L_FLAG;
                break;
            case 'R':
                *opt_flag |= MINILS_OPT_R_FLAG;
                break;
            case 'i':
                *opt_flag |= MINILS_OPT_I_FLAG;
                break;
            default:
                return -1;
        }
    }
    posarg_idices[pos_arg_idx] = -1;

    return 0;
}

int minils_get_posargs_len(int* posarg_indices, int max_posargs) {
    int len = 0;

    for (int i = 0; i < max_posargs && posarg_indices[i] != -1; ++i) {
        ++len;
    }
    return len;
}