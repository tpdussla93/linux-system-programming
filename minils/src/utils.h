#ifndef MINILS_SRC_UTILS_H_
#define MINILS_SRC_UTILS_H_

#include <time.h>

char* minils_get_permstr(char perm_str[11], int st_mode);

int minils_get_devnum_str(char* devnum_str, size_t buf_size, int devnum);

size_t minils_get_timestr(char* time_str, size_t buf_size,
                          time_t time_since_epoch);
#endif  // MINILS_SRC_UTILS_H_