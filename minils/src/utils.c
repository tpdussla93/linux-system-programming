#include "src/utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>

static const char rwx[] = "rwx";
static const char sst[] = "sst";

char* minils_get_permstr(char permstr[11], int st_mode) {
    strcpy(permstr, "----------");

    // set file type
    switch (st_mode & S_IFMT) {
        case S_IFBLK:
            permstr[0] = 'b';
            break;
        case S_IFCHR:
            permstr[0] = 'c';
            break;
        case S_IFDIR:
            permstr[0] = 'd';
            break;
        case S_IFIFO:
            permstr[0] = 'p';
            break;
        case S_IFLNK:
            permstr[0] = 'l';
            break;
        case S_IFREG:
            permstr[0] = '-';
            break;
        case S_IFSOCK:
            permstr[0] = 's';
            break;
        default:
            permstr[0] = '?';
            break;
    }

    for (int i = 0; i < 9; ++i) {
        int is_set_special = (st_mode >> (11 - i / 3)) & 0x1;
        int is_x_bit = (i + 1) % 3 == 0;

        if ((st_mode >> (8 - i)) & 0x1) {
            permstr[i + 1] = rwx[i % 3];
            if (is_x_bit) {
                if (is_set_special) {
                    permstr[i + 1] = sst[i / 3];
                }
            }
        } else if (is_x_bit && is_set_special) {
            permstr[i + 1] = sst[i / 3] + ('A' - 'a');
        }
    }

    return permstr;
}