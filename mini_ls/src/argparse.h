#ifndef MINI_LS_SRC_ARGPARSE_H_
#define MINI_LS_SRC_ARGPARSE_H_

typedef int opt_flag_t;

#define OPT_A_FLAG 0x01
#define OPT_L_FLAG 0x02
#define OPT_R_FLAG 0x04
#define OPT_I_FLAG 0x08

#define OPT_IS_A_SET(flag) ((flag)&OPT_A_FLAG)
#define OPT_IS_L_SET(flag) ((flag)&OPT_L_FLAG)
#define OPT_IS_R_SET(flag) ((flag)&OPT_R_FLAG)
#define OPT_IS_I_SET(flag) ((flag)&OPT_I_FLAG)

// return 0 if success
int parse_args(int argc, char* const* argv, opt_flag_t* opt_flag,
               int* posarg_idices, int max_posargs);

int get_posargs_len(int* posarg_indices, int max_posargs);

#endif  // MINI_LS_SRC_ARGPARSE_H_