#ifndef MINILS_SRC_ARGPARSE_H_
#define MINILS_SRC_ARGPARSE_H_

typedef int minils_opt_flag_t;

#define MINILS_OPT_A_FLAG 0x01
#define MINILS_OPT_L_FLAG 0x02
#define MINILS_OPT_R_FLAG 0x04
#define MINILS_OPT_I_FLAG 0x08

#define MINILS_OPT_IS_A_SET(flag) ((flag)&MINILS_OPT_A_FLAG)
#define MINILS_OPT_IS_L_SET(flag) ((flag)&MINILS_OPT_L_FLAG)
#define MINILS_OPT_IS_R_SET(flag) ((flag)&MINILS_OPT_R_FLAG)
#define MINILS_OPT_IS_I_SET(flag) ((flag)&MINILS_OPT_I_FLAG)

// return 0 if success
int minils_parse_args(int argc, char* const* argv, minils_opt_flag_t* opt_flag,
                      int* posarg_idices, int max_posargs);

int minils_get_posargs_len(int* posarg_indices, int max_posargs);

#endif  // MINILS_SRC_ARGPARSE_H_