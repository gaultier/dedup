#pragma once

#include <stdio.h>

#include "files.h"
#include "image.h"
#include "options.h"
#include "thread_pool.h"
#include "utils.h"

typedef struct {
    u64 ahash;
    SDL_Surface* surface_src;
    SDL_Surface* surface_thumbnail;
    SDL_Surface* surface_hash;
} image;

typedef struct {
    char file_name[MAX_FILE_NAME_LEN];
    u8 method;
    union {
        u32 non_crypt_hash;
        image img;
    } h;
} file_hash;

typedef struct {
    file_hash* data;
    usize len;
    usize bytes_read;
    usize files_visited;
    thread_pool* pool;
} file_hashes_buffer;

static void file_hash_compute(void* arg) {
    file_hash* f_hash = arg;

    FILE* f = fopen(f_hash->file_name, "rb");
    if (!f) {
        LOG_ERR("Could not open file `%s`, skipping: %s\n", f_hash->file_name,
                strerror(errno));
        return;
    }

    usize file_content_len;
    pg_assert_int(file_info(f_hash->file_name, &file_content_len, NULL, NULL),
                  ==, 0);

    u8* file_content = pg_malloc(file_content_len);
    if (fread(file_content, 1, file_content_len, f) != file_content_len) {
        LOG_ERR(
            "Could not read the entirety of the contents for the file "
            "`%s`, skipping: %s\n",
            f_hash->file_name, strerror(errno));

        fclose(f);
        return;
    }

    fclose(f);

    if (img_ahash(f_hash->file_name, file_content, file_content_len,
                  &f_hash->h.img.ahash, &f_hash->h.img.surface_src,
                  &f_hash->h.img.surface_thumbnail,
                  &f_hash->h.img.surface_hash)) {
        f_hash->method = METHOD_AHASH;
        LOG("%s: ahash=%llx\n\n", f_hash->file_name, f_hash->h.img.ahash);
    } else {
        f_hash->h.non_crypt_hash =
            murmur3_32(file_content, file_content_len, 0);
        LOG("%s: non cryptographic hash=%x\n\n", f_hash->file_name,
            f_hash->h.non_crypt_hash);

        f_hash->method = METHOD_NON_CRYPT_HASH;
    }

    pg_free(file_content);
}

static void file_names_collect(const char* path, usize size, void* arg) {
    file_hashes_buffer* const buffer = arg;

    if (buffer->len >= MAX_FILES) {
        fprintf(stderr,
                "%s:%d:Too many files encountered: %zu >= %u, skipping "
                "remaining files\n",
                __FILE__, __LINE__, buffer->len, MAX_FILES);
        return;
    }

    if (size > opts.size) {
        LOG("file %s too big (%zu > %zu), ignoring\n", path, size, opts.size);
        return;
    }

    LOG("dir walk: seen file %s\n", path);
    file_hash* const f_hash = buffer->data + buffer->len;
    memset(f_hash, 0, sizeof(file_hash));
    memcpy(f_hash->file_name, path, strlen(path));
    buffer->len += 1;

    buffer->bytes_read += (usize)size;
    buffer->files_visited += 1;

    thread_pool_push(
        buffer->pool,
        (thread_pool_work_item){.arg = buffer->data[buffer->len - 1].file_name,
                                .fn = file_hash_compute});
}

void files_scan(file_hashes_buffer* file_hashes, file_hashes_buffer* matches,
                options* opts) {
    thread_pool_start(file_hashes->pool);

    dir_walk(opts->dir, file_names_collect, file_hashes);

    thread_pool_finish(file_hashes->pool);

    for (usize i = 0; i < file_hashes->len; i++) {
        const file_hash* const f_hash_i = &file_hashes->data[i];
        for (usize j = 0; j < i; j++) {
            const file_hash* const f_hash_j = &file_hashes->data[j];

            if (f_hash_i->method == METHOD_NON_CRYPT_HASH &&
                f_hash_i->method == f_hash_j->method &&
                f_hash_i->h.non_crypt_hash == f_hash_j->h.non_crypt_hash) {
                printf("%s\n%s\nidentical non cryptographic hashes: %x\n\n",
                       f_hash_i->file_name, f_hash_j->file_name,
                       f_hash_i->h.non_crypt_hash);
            } else if (f_hash_i->method == METHOD_AHASH &&
                       f_hash_i->method == f_hash_j->method) {
                const u32 dist = hamming_distance(f_hash_i->h.img.ahash,
                                                  f_hash_j->h.img.ahash);
                if (dist <= opts->distance) {
                    printf(
                        "%s\n%s\nsimilar ahashes: %llx %llx distance: %u\n\n",
                        f_hash_i->file_name, f_hash_j->file_name,
                        f_hash_i->h.img.ahash, f_hash_j->h.img.ahash, dist);

                    pg_assert_uint64(
                        matches->len, <,
                        file_hashes->len * (file_hashes->len - 1) - 2);
                    memcpy(&matches->data[matches->len++], f_hash_i,
                           sizeof(file_hash));
                    memcpy(&matches->data[matches->len++], f_hash_j,
                           sizeof(file_hash));
                }
            }
        }
    }
}
