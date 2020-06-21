#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "options.h"
static options opts;

#define SDL_MAIN_HANDLED
#include "config.h"
#include "file_hash.h"
#include "files.h"
#include "thread_pool.h"
#include "ui.h"
#include "utils.h"

void image_next(usize matches_count, usize* i) {
    *i = *i == (matches_count - 1) ? 0 : *i + 1;
}

void image_previous(usize matches_count, usize* i) {
    *i = *i == 0 ? (matches_count - 1) : *i - 1;
}

int main(int argc, char* argv[]) {
    (void)argc;

    options_parse(argv, &opts);
    log_verbose = opts.verbose;

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) == 0)
        DIE(EINVAL, "Could not initialize SDL2_image: %s\n", IMG_GetError());

    usize num_cores = cores_count();
    pg_assert_uint64(num_cores, >, 0);

    thread_pool pool;
    thread_pool_init(&pool, num_cores);
    thread_pool_start(&pool);

    file_hashes_buffer file_hashes = {
        .data = pg_malloc(MAX_FILES * sizeof(file_hash)), .pool = &pool};
    dir_walk(opts.dir, file_names_collect, &file_hashes);

    thread_pool_finish(&pool);

    file_hashes_buffer matches = {
        .data = pg_malloc(file_hashes.len * (file_hashes.len - 1) *
                          sizeof(file_hash))};

    for (usize i = 0; i < file_hashes.len; i++) {
        const file_hash* const f_hash_i = &file_hashes.data[i];
        for (usize j = 0; j < i; j++) {
            const file_hash* const f_hash_j = &file_hashes.data[j];

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
                if (dist <= opts.distance) {
                    printf(
                        "%s\n%s\nsimilar ahashes: %llx %llx distance: %u\n\n",
                        f_hash_i->file_name, f_hash_j->file_name,
                        f_hash_i->h.img.ahash, f_hash_j->h.img.ahash, dist);

                    pg_assert_uint64(
                        matches.len, <,
                        file_hashes.len * (file_hashes.len - 1) - 2);
                    memcpy(&matches.data[matches.len++], f_hash_i,
                           sizeof(file_hash));
                    memcpy(&matches.data[matches.len++], f_hash_j,
                           sizeof(file_hash));
                }
            }
        }
    }
    printf("Files scanned: ");
    print_humanize_usize(file_hashes.files_visited, "");
    printf("Bytes scanned: ");
    print_humanize_usize(file_hashes.bytes_read, "b");

    if (matches.len == 0 || opts.windowless) return 0;

    SDL_Window* window;
    ui_init(&window);

    ui_run(window, &matches);
}
