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

int main(int argc, char* argv[]) {
    (void)argc;

    options_parse(argv, &opts);
    log_verbose = opts.verbose;

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF) == 0)
        DIE(EINVAL, "Could not initialize SDL2_image: %s\n", IMG_GetError());

    thread_pool pool;
    usize num_cores = cores_count();
    pg_assert_uint64(num_cores, >, 0);
    thread_pool_init(&pool, num_cores);

    file_hashes_buffer file_hashes = {
        .data = pg_malloc(MAX_FILES * sizeof(file_hash)), .pool = &pool};

    file_hashes_buffer matches = {
        .data = pg_malloc((usize)MAX_FILES * sizeof(file_hash))};

    files_scan(&file_hashes, &matches, &opts);

    printf("Files scanned: ");
    print_humanize_usize(file_hashes.files_visited, "");
    printf("Bytes scanned: ");
    print_humanize_usize(file_hashes.bytes_read, "b");

    if (opts.windowless) return 0;

    SDL_Window* window;
    void* ctx = ui_init(&window);

    ui_run(window, ctx, &matches);
}
