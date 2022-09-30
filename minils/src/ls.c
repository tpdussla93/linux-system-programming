#include "src/ls.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "src/argparse.h"
#include "src/utils.h"

typedef struct cui_align {
    int max_inode_len;
    int max_nlink_len;
    int max_uname_len;
    int max_gname_len;
    int max_filesize_len;
    int max_time_len;
} cui_align_t;

static void print_path(char const* dirname, char const* path,
                       struct stat* stat_buf, minils_opt_flag_t opt_flag,
                       const cui_align_t* cui_align) {
    if (path == NULL) {
        return;
    }
    if (!MINILS_OPT_IS_A_SET(opt_flag) && path[0] == '.') {
        return;
    }

    int is_i_set = MINILS_OPT_IS_I_SET(opt_flag);

    char permstr[11];
    char buf[FILENAME_MAX + 1];
    struct passwd* pwd;
    struct group* grp;

    if (MINILS_OPT_IS_L_SET(opt_flag)) {
        if (is_i_set) {
            // print inode
            printf("%*lu ", cui_align->max_inode_len, stat_buf->st_ino);
        }

        // print permission
        printf("%s ", minils_get_permstr(permstr, stat_buf->st_mode));

        // print num link
        printf("%*lu ", cui_align->max_nlink_len, stat_buf->st_nlink);

        // print owner
        pwd = getpwuid(stat_buf->st_uid);
        printf("%*s ", cui_align->max_uname_len, pwd->pw_name);

        // print group
        grp = getgrgid(stat_buf->st_gid);
        printf("%*s ", cui_align->max_gname_len, grp->gr_name);

        // print filesize
        if (permstr[0] == 'c' || permstr[0] == 'b') {
            if (minils_get_devnum_str(buf, sizeof buf, stat_buf->st_rdev) > 0) {
                printf("%*s ", cui_align->max_filesize_len, buf);
            } else {
                printf("%*s ", cui_align->max_filesize_len, "?, ?");
            }
        } else {
            printf("%*lu ", cui_align->max_filesize_len, stat_buf->st_size);
        }

        // print modification time
        if (minils_get_timestr(buf, sizeof buf, stat_buf->st_mtim.tv_sec)) {
            printf("%*s ", cui_align->max_time_len, buf);
        } else {
            printf("%*s ", cui_align->max_time_len, "???");
        }

        // print path
        if (S_ISLNK(stat_buf->st_mode)) {
            char full_path[FILENAME_MAX + 1];

            if (dirname) {
                int ret =
                    snprintf(full_path, FILENAME_MAX, "%s/%s", dirname, path);
                if (ret < 0) {
                    perror("snprintf failed: ");
                    strncpy(full_path, path, FILENAME_MAX);
                }
                full_path[ret] = 0;
            } else {
                strncpy(full_path, path, FILENAME_MAX);
            }

            ssize_t len;
            if ((len = readlink(full_path, buf, FILENAME_MAX)) != -1) {
                buf[len] = 0;
                printf("%s -> %s\n", path, buf);
            } else {
                perror("readlink fail: ");
                printf("%s -> ?\n", path);
            }
        } else {
            printf("%s\n", path);
        }
    } else {
        if (is_i_set) {
            printf("%lu ", stat_buf->st_ino);
        }
        printf("%s  ", path);
    }
}

static void find_cui_align(char* paths[], int num_paths,
                           cui_align_t* cui_align) {
    memset(cui_align, 0, sizeof *cui_align);

    char buf[1024];

    struct stat stat_buf;
    struct passwd* pwd;
    struct group* grp;

    size_t len = 0;
    for (int i = 0; i < num_paths; ++i) {
        if (lstat(paths[i], &stat_buf)) {
            perror(paths[i]);
            continue;
        }

        if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_ino) > 0) {
            len = strlen(buf);
            cui_align->max_inode_len =
                cui_align->max_inode_len < len ? len : cui_align->max_inode_len;
        }

        if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_nlink) > 0) {
            len = strlen(buf);
            cui_align->max_nlink_len =
                cui_align->max_nlink_len < len ? len : cui_align->max_nlink_len;
        }

        if ((pwd = getpwuid(stat_buf.st_uid)) != NULL) {
            len = strlen(pwd->pw_name);
            cui_align->max_uname_len =
                cui_align->max_uname_len < len ? len : cui_align->max_uname_len;
        }

        if ((grp = getgrgid(stat_buf.st_gid)) != NULL) {
            len = strlen(grp->gr_name);
            cui_align->max_gname_len =
                cui_align->max_gname_len < len ? len : cui_align->max_gname_len;
        }

        if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)) {
            if (minils_get_devnum_str(buf, sizeof buf, stat_buf.st_rdev) > 0) {
                len = strlen(buf);
                cui_align->max_filesize_len = cui_align->max_filesize_len < len
                                                  ? len
                                                  : cui_align->max_filesize_len;
            }
        } else {
            if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_size) > 0) {
                len = strlen(buf);
                cui_align->max_filesize_len = cui_align->max_filesize_len < len
                                                  ? len
                                                  : cui_align->max_filesize_len;
            }
        }

        if (minils_get_timestr(buf, sizeof buf, stat_buf.st_mtim.tv_sec) > 0) {
            len = strlen(buf);
            cui_align->max_time_len =
                cui_align->max_time_len < len ? len : cui_align->max_time_len;
        }
    }
}

static void find_cui_align_from_dp(DIR* dp, char const* dirname,
                                   cui_align_t* cui_align) {
    memset(cui_align, 0, sizeof *cui_align);

    char buf[FILENAME_MAX];

    struct stat stat_buf;
    struct passwd* pwd;
    struct group* grp;

    size_t len = 0;
    struct dirent* p;
    while ((p = readdir(dp)) != NULL) {
        snprintf(buf, sizeof buf, "%s/%s", dirname, p->d_name);
        if (lstat(buf, &stat_buf)) {
            perror(buf);
            continue;
        }

        if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_ino) > 0) {
            len = strlen(buf);
            cui_align->max_inode_len =
                cui_align->max_inode_len < len ? len : cui_align->max_inode_len;
        }

        if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_nlink) > 0) {
            len = strlen(buf);
            cui_align->max_nlink_len =
                cui_align->max_nlink_len < len ? len : cui_align->max_nlink_len;
        }

        if ((pwd = getpwuid(stat_buf.st_uid)) != NULL) {
            len = strlen(pwd->pw_name);
            cui_align->max_uname_len =
                cui_align->max_uname_len < len ? len : cui_align->max_uname_len;
        }

        if ((grp = getgrgid(stat_buf.st_gid)) != NULL) {
            len = strlen(grp->gr_name);
            cui_align->max_gname_len =
                cui_align->max_gname_len < len ? len : cui_align->max_gname_len;
        }

        if (S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode)) {
            if (minils_get_devnum_str(buf, sizeof buf, stat_buf.st_rdev) > 0) {
                len = strlen(buf);
                cui_align->max_filesize_len = cui_align->max_filesize_len < len
                                                  ? len
                                                  : cui_align->max_filesize_len;
            }
        } else {
            if (snprintf(buf, sizeof buf, "%lu", stat_buf.st_size) > 0) {
                len = strlen(buf);
                cui_align->max_filesize_len = cui_align->max_filesize_len < len
                                                  ? len
                                                  : cui_align->max_filesize_len;
            }
        }

        if (minils_get_timestr(buf, sizeof buf, stat_buf.st_mtim.tv_sec) > 0) {
            len = strlen(buf);
            cui_align->max_time_len =
                cui_align->max_time_len < len ? len : cui_align->max_time_len;
        }
    }
}

static size_t find_total_from_dp(DIR* dp, char const* dirname,
                                 minils_opt_flag_t opt_flag) {
    struct dirent* p;
    struct stat stat_buf;
    char buf[FILENAME_MAX];
    size_t total = 0;
    while ((p = readdir(dp)) != NULL) {
        snprintf(buf, sizeof buf, "%s/%s", dirname, p->d_name);
        if (lstat(buf, &stat_buf)) {
            perror(buf);
            continue;
        }

        if (MINILS_OPT_IS_A_SET(opt_flag)) {
            total += stat_buf.st_blocks / 2;
        } else if (p->d_name[0] != '.') {
            total += stat_buf.st_blocks / 2;
        }
    }
    return total;
}

static void ls_dir(char const* dir, minils_opt_flag_t opt_flag) {
    DIR* dp;
    struct dirent* p;
    struct stat stat_buf;
    cui_align_t cui_align;
    char buf[FILENAME_MAX];

    if ((dp = opendir(dir)) == NULL) {
        perror(dir);
        return;
    };

    if (MINILS_OPT_IS_L_SET(opt_flag)) {
        find_cui_align_from_dp(dp, dir, &cui_align);
        rewinddir(dp);

        printf("total: %lu\n", find_total_from_dp(dp, dir, opt_flag));
        rewinddir(dp);
    }

    if (MINILS_OPT_IS_R_SET(opt_flag)) {
        printf("%s:\n", dir);
    }
    while ((p = readdir(dp)) != NULL) {
        snprintf(buf, sizeof buf, "%s/%s", dir, p->d_name);
        if (lstat(buf, &stat_buf)) {
            perror(buf);
            continue;
        }
        print_path(dir, p->d_name, &stat_buf, opt_flag, &cui_align);
    }
    if (!MINILS_OPT_IS_L_SET(opt_flag)) {
        printf("\n");
    }
    if (MINILS_OPT_IS_R_SET(opt_flag)) {
        printf("\n");
    }

    if (MINILS_OPT_IS_R_SET(opt_flag)) {
        rewinddir(dp);

        while ((p = readdir(dp)) != NULL) {
            snprintf(buf, sizeof buf, "%s/%s", dir, p->d_name);
            if (lstat(buf, &stat_buf)) {
                perror(buf);
                continue;
            }

            if (S_ISDIR(stat_buf.st_mode) && strcmp(p->d_name, ".") &&
                strcmp(p->d_name, "..")) {
                if (!MINILS_OPT_IS_A_SET(opt_flag) && p->d_name[0] == '.') {
                    continue;
                }
                ls_dir(buf, opt_flag);
            }
        }
    }
    closedir(dp);
}

void minils_ls(char* paths[], int num_paths, minils_opt_flag_t opt_flag) {
    struct stat stat_buf;
    cui_align_t cui_align;

    if (num_paths == 0) {
        ls_dir(".", opt_flag);
    } else if (num_paths == 1) {
        if (lstat(paths[0], &stat_buf)) {
            perror(paths[0]);
            return;
        }
        if (S_ISDIR(stat_buf.st_mode)) {
            ls_dir(paths[0], opt_flag);
        } else {
            memset(&cui_align, 0, sizeof cui_align);
            print_path(NULL, paths[0], &stat_buf, opt_flag, &cui_align);
            if (!MINILS_OPT_IS_L_SET(opt_flag)) {
                printf("\n");
            }
        }
    } else if (num_paths > 0) {
        if (MINILS_OPT_IS_L_SET(opt_flag)) {
            find_cui_align(paths, num_paths, &cui_align);
        }

        int non_dir_printed = 0;
        for (int i = 0; i < num_paths; ++i) {
            if (lstat(paths[i], &stat_buf)) {
                perror(paths[i]);
                continue;
            }
            if (!S_ISDIR(stat_buf.st_mode)) {
                non_dir_printed = 1;
                print_path(NULL, paths[i], &stat_buf, opt_flag, &cui_align);
            }
        }
        if (non_dir_printed) {
            if (!MINILS_OPT_IS_L_SET(opt_flag)) {
                printf("\n\n");
            } else {
                printf("\n");
            }
        }

        // list dir
        for (int i = 0; i < num_paths; ++i) {
            if (lstat(paths[i], &stat_buf)) {
                perror(paths[i]);
                continue;
            }
            if (S_ISDIR(stat_buf.st_mode)) {
                printf("%s:\n", paths[i]);
                ls_dir(paths[i], opt_flag);
                printf("\n");
            }
        }
    } else {
        fprintf(stderr, "invalid num_paths for minils_ls\n");
    }
}