#pragma once

#include <dirent.h>

#include "utils.h"

#define METHOD_NONE 0
#define METHOD_NON_CRYPT_HASH 1
#define METHOD_AHASH 2

typedef void (*dir_walk_fn)(const char*, usize, void*);

static void dir_walk(const char* path, dir_walk_fn fn, void* arg) {
    bool is_dir, is_regular_file;
    usize size;
    if (file_info(path, &size, &is_dir, &is_regular_file) != 0) {
        return;
    }

    if (is_regular_file) {
        fn(path, size, arg);

        return;
    }

    DIR* dirp = opendir(path);
    if (dirp == NULL) {
        LOG_ERR("Could not open `%s`: %s. Skipping.\n", path, strerror(errno));
        return;
    }

    struct dirent* entry;
    char pathbuf[MAX_FILE_NAME_LEN + 1] = {0};
    usize path_len = strlen(path);
    pg_assert_uint64(path_len, <=, MAX_FILE_NAME_LEN);
    memcpy(pathbuf, path, path_len);

    usize j = path_len && path[path_len - 1] == '/' ? path_len - 1 : path_len;

    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        pathbuf[j] = '/';
        strcpy(pathbuf + j + 1, entry->d_name);
        LOG_DEBUG("dir walk: pathbuf %s\n", pathbuf);

        LOG_DEBUG("dir walk: seen file %s\n", entry->d_name);

        dir_walk(pathbuf, fn, arg);
    }

    closedir(dirp);
}
