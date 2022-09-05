#include <stdio.h>

#include "src/argparse.h"
#include "src/utils.h"

#define MAX_POSARGS 1024

int main(int argc, char* argv[]) {
    int posarg_idices[MAX_POSARGS];
    opt_flag_t opt_flag = 0;

    if (minils_parse_args(argc, argv, &opt_flag, posarg_idices,
                          sizeof(posarg_idices))) {
        fprintf(stderr, "Failed to parse arguments\n");
        return -1;
    }

    int num_posargs =
        minils_get_posargs_len(posarg_idices, sizeof(posarg_idices));

    for (int i = 0; i < num_posargs; ++i) {
        printf("pos[%d] = %s\n", i, argv[posarg_idices[i]]);
    }
    printf("\n");

    printf("options\n");
    if (MINILS_OPT_IS_A_SET(opt_flag)) {
        printf("a flag set\n");
    }
    if (MINILS_OPT_IS_L_SET(opt_flag)) {
        printf("l flag set\n");
    }
    if (MINILS_OPT_IS_R_SET(opt_flag)) {
        printf("R flag set\n");
    }
    if (MINILS_OPT_IS_I_SET(opt_flag)) {
        printf("i flag set\n");
    }
    printf("\n");

    char permstr[11];

    for (int st_mode = 0; st_mode <= 0777; ++st_mode) {
        minils_get_permstr(permstr, st_mode + 07000);
        printf("%03o -> %s\n", st_mode, permstr);
    }

    return 0;
}