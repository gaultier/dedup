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
#include "utils.h"
#include "window.h"

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

    if (TTF_Init() == -1)
        DIE(EINVAL, "Could not initialize TTF: %s\n", TTF_GetError());

    TTF_Font* font;
    font = TTF_OpenFont("font.ttf", 24);
    if (!font) font = TTF_OpenFont("/usr/local/share/font.ttf", 24);
    if (!font) DIE(EINVAL, "Could not open font: %s\n", TTF_GetError());

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
    SDL_Renderer* renderer;
    window_create(&window, &renderer);

    SDL_Color color_white = {255, 255, 255, 0};

    i32 win_w, win_h;
    const u8 font_size = 40;

    usize i = 0;

    file_hash* f_hash = &matches.data[0];

    SDL_Texture* texture_thumbnail =
        SDL_CreateTextureFromSurface(renderer, f_hash->h.img.surface_thumbnail);
    if (!texture_thumbnail)
        DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
            f_hash->file_name, SDL_GetError());

    SDL_Texture* texture_hash =
        SDL_CreateTextureFromSurface(renderer, f_hash->h.img.surface_hash);
    if (!texture_hash)
        DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
            f_hash->file_name, SDL_GetError());

    SDL_Texture* texture_src =
        SDL_CreateTextureFromSurface(renderer, f_hash->h.img.surface_src);
    if (!texture_src)
        DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
            f_hash->file_name, SDL_GetError());

    while (1) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) return 0;

        SDL_GetRendererOutputSize(renderer, &win_w, &win_h);

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    return 0;
                    break;
                case SDLK_LEFT: {
                    image_next(matches.len, &i);

                    break;
                }
                case SDLK_RIGHT: {
                    image_previous(matches.len, &i);
                    break;
                }
            }
        }
        SDL_DestroyTexture(texture_thumbnail);
        SDL_DestroyTexture(texture_hash);
        SDL_DestroyTexture(texture_src);

        f_hash = &matches.data[i];
        texture_thumbnail = SDL_CreateTextureFromSurface(
            renderer, f_hash->h.img.surface_thumbnail);
        if (!texture_thumbnail)
            DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
                f_hash->file_name, SDL_GetError());

        texture_hash =
            SDL_CreateTextureFromSurface(renderer, f_hash->h.img.surface_hash);
        if (!texture_hash)
            DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
                f_hash->file_name, SDL_GetError());

        texture_src =
            SDL_CreateTextureFromSurface(renderer, f_hash->h.img.surface_src);
        if (!texture_src)
            DIE(EINVAL, "Could not create the texture for `%s`: %s\n",
                f_hash->file_name, SDL_GetError());

        if (SDL_RenderClear(renderer) == -1)
            DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());

        const i32 sep_size = 20;
        const i32 square_size = win_w / 8;
        SDL_Rect r = {.x = (win_w - (sep_size + 2 * square_size)) / 2,
                      .y = sep_size,
                      .w = square_size,
                      .h = square_size};
        if (SDL_RenderCopy(renderer, texture_thumbnail, NULL, &r) == -1)
            DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());

        r = (SDL_Rect){.x = (win_w - (sep_size + 2 * square_size)) / 2 +
                            sep_size + square_size,
                       .y = sep_size,
                       .w = square_size,
                       .h = square_size};
        if (SDL_RenderCopy(renderer, texture_hash, NULL, &r) == -1)
            DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());

        r = (SDL_Rect){.x = sep_size,
                       .y = 2 * sep_size + square_size,
                       .w = win_w - 2 * sep_size,
                       .h = win_h - 6 * sep_size - 2 * sep_size - square_size};
        if (SDL_RenderCopy(renderer, texture_src, NULL, &r) == -1)
            DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());

        SDL_Rect rect_text = {.w = win_w - 2 * sep_size,
                              .h = font_size,
                              .x = sep_size,
                              .y = win_h - font_size - sep_size};
        static char path_string[MAX_FILE_NAME_LEN + 10] = {0};
        snprintf(path_string, ARR_SIZE(path_string), "Path: %s",
                 f_hash->file_name);
        SDL_Texture* texture_path =
            window_text(renderer, path_string, font, &color_white, &rect_text);

        SDL_Rect rect_ahash = {.w = 2 * square_size,
                               .h = font_size,
                               .x = sep_size,
                               .y = win_h - font_size - 3 * sep_size};
        static char ahash_string[256] = {0};
        snprintf(ahash_string, ARR_SIZE(ahash_string), "Hash: %llu",
                 f_hash->h.img.ahash);
        SDL_Texture* texture_ahash = window_text(renderer, ahash_string, font,
                                                 &color_white, &rect_ahash);

        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(texture_path);
        SDL_DestroyTexture(texture_ahash);
    }
}
