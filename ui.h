#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

#include "options.h"

#pragma once
#define GL_SILENCE_DEPRECATION 1
#include <GL/glew.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#undef NK_ASSERT
#define NK_SDL_GL3_IMPLEMENTATION
#include "vendor/nuklear.h"
#include "vendor/nuklear_sdl_gl3.h"

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#include "file_hash.h"

struct ui_state {
    char text_buffer[MAX_FILE_NAME_LEN * 2 + 10];
    i32 *img_selected;
    i32 img_current;

    bool is_popup_active;
    usize user_path_capacity;
    char *user_path;
};

static void ui_state_init(struct ui_state *state, usize matches_len) {
    state->img_selected = pg_malloc(sizeof(i32) * matches_len);
    memset(state->img_selected, 0, sizeof(i32) * matches_len);

    state->img_current = 0;
    state->img_selected[state->img_current] = 1;
    state->is_popup_active = (matches_len == 0);

    state->user_path_capacity = 20000;
    state->user_path = pg_malloc(state->user_path_capacity);
    state->user_path[0] = 0;
}

static void ui_set_style(struct nk_context *ctx) {
    struct nk_color table[NK_COLOR_COUNT];
    table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
    table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 215);
    table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 220);
    table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
    table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
    table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
    table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
    table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
    table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
    table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
    table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
    table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
    table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
    table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
    table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
    table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
    table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
    table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
    table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
    table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 220);
    nk_style_from_table(ctx, table);
}

static bool ui_texture_load(SDL_Surface *surface, GLuint *texture_id,
                            const char *path) {
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    // FIXME: format

    GLenum gl_format = 0;
    if (surface->format->BytesPerPixel == 3)
        gl_format = GL_RGB;
    else if (surface->format->BytesPerPixel == 4)
        gl_format = GL_RGBA;
    else {  // TODO: add more formats
        LOG_ERR(
            "I do not know how to map the format %d to OpenGL (from file "
            "`%s`)\n",
            surface->format->BytesPerPixel, path);

        glDeleteTextures(1, texture_id);
        return false;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                 gl_format, GL_UNSIGNED_BYTE, surface->pixels);

    return true;
}

static void *ui_init(SDL_Window **window) {
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    *window = SDL_CreateWindow("dedup", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, 1024, 768,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!*window) DIE(EINVAL, "Could not create window: %s\n", SDL_GetError());

    SDL_GLContext *context = SDL_GL_CreateContext(*window);
    if (!context)
        DIE(EINVAL, "Could not initialize OpenGL: %s\n", SDL_GetError());

    glewExperimental = 1;
    if (glewInit() != GLEW_OK) DIE(EINVAL, "Failed to setup GLEW%s\n", "");

    printf("GL version=%s\n", glGetString(GL_VERSION));
    printf("GL shading language version=%s\n",
           glGetString(GL_SHADING_LANGUAGE_VERSION));

    i32 window_width, window_height;
    SDL_GetWindowSize(*window, &window_width, &window_height);
    glViewport(0, 0, window_width, window_height);

    struct nk_context *ctx = nk_sdl_init(*window);
    {
        struct nk_font_atlas *atlas;
        nk_sdl_font_stash_begin(&atlas);
        nk_sdl_font_stash_end();
    }
    ui_set_style(ctx);
    return ctx;
}

static void ui_delete_match(file_hashes_buffer *matches, usize i) {
    pg_assert_uint64(i, >=, 0);
    pg_assert_uint64(i, <, matches->len - 1);

    memcpy(&matches->data[i], &matches->data[matches->len - 2],
           sizeof(file_hash));
    memcpy(&matches->data[i + 1], &matches->data[matches->len - 1],
           sizeof(file_hash));

    matches->len = matches->len == 2 ? 0 : matches->len - 2;
}

static void ui_on_click_delete(file_hashes_buffer *matches, usize i,
                               const char *path_to_delete) {
    pg_assert_int(i % 2, ==, 0);

    file_move_to_trash(path_to_delete);
    ui_delete_match(matches, i);
}

static void ui_textures_load(GLuint *texture_ids, usize *len,
                             file_hashes_buffer *matches) {
    while (*len < matches->len) {
        if (!ui_texture_load(matches->data[*len].h.img.surface_src,
                             &texture_ids[*len],
                             matches->data[*len].file_name) ||
            !ui_texture_load(matches->data[*len + 1].h.img.surface_src,
                             &texture_ids[*len + 1],
                             matches->data[*len].file_name)) {
            LOG("Could not generate OpenGL texture for `%s` and `%s`, "
                "skipping\n",
                matches->data[*len].file_name,
                matches->data[*len + 1].file_name);
            ui_delete_match(matches, *len);
        } else {
            *len += 2;
        }
    }
}

static void ui_run(SDL_Window *window, void *nuklear_ctx,
                   file_hashes_buffer *file_hashes, file_hashes_buffer *matches,
                   options *opts) {
    struct nk_context *ctx = nuklear_ctx;
    pg_assert_uint64(matches->len, >=, 2);

    // OpenGL textures
    GLuint *texture_ids = pg_malloc(sizeof(GLuint) * matches->len);
    usize texture_ids_len = 0;
    ui_textures_load(texture_ids, &texture_ids_len, matches);

    struct ui_state state;
    ui_state_init(&state, matches->len);

    // Main loop
    for (;;) {
        i32 window_width, window_height;

        SDL_Event event;
        nk_input_begin(ctx);
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT) return;

        nk_sdl_handle_event(&event);
        nk_input_end(ctx);

        SDL_GetWindowSize(window, &window_width, &window_height);

        if (nk_begin(ctx, "Image Deduper",
                     nk_rect(0, 0, window_width, window_height),
                     NK_WINDOW_BORDER)) {
            nk_layout_row_begin(ctx, NK_DYNAMIC, window_height - 30, 2);
            nk_layout_row_push(ctx, 0.25f);
            if (nk_group_begin(ctx, "Preview", 0)) {
                nk_layout_row_dynamic(ctx, 0, 1);
                if (state.is_popup_active) {
                    struct nk_rect popup_rect = {.x = (window_width - 400) / 2,
                                                 .y = (window_height - 110) / 3,
                                                 .w = 400,
                                                 .h = 110};

                    nk_popup_begin(ctx, NK_POPUP_DYNAMIC, "Path",
                                   NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER,
                                   popup_rect);
                    nk_layout_row_dynamic(ctx, 0, 1);
                    nk_label(ctx, "Directory to scan:", NK_TEXT_LEFT);
                    nk_edit_string_zero_terminated(
                        ctx, NK_EDIT_FIELD | NK_EDIT_CLIPBOARD, state.user_path,
                        state.user_path_capacity, nk_filter_default);

                    if (strlen(state.user_path) > 0 &&
                        nk_button_label(ctx, "Scan!")) {
                        nk_popup_close(ctx);
                        state.is_popup_active = false;

                        opts->dir = state.user_path;
                        files_scan(file_hashes, matches, opts);
                        // TODO: regen OpenGL textures
                        memset(state.user_path, 0, state.user_path_capacity);
                    }

                    nk_popup_end(ctx);
                }

                if (nk_button_label(ctx, "Pick a directory to scan")) {
                    state.is_popup_active = true;
                }

                for (usize i = 0; i < matches->len; i += 2) {
                    file_hash *f_hash_1 = &matches->data[i];
                    SDL_Surface *surface_1 = f_hash_1->h.img.surface_src;

                    nk_layout_row_dynamic(ctx, 50, 1);

                    struct nk_image img = {
                        .handle = {.id = (i32)texture_ids[i]},
                        .w = surface_1->w,
                        .h = surface_1->h,
                        .region = {0, 0, surface_1->w, surface_1->h}};

                    file_hash *f_hash_2 = &matches->data[i + 1];
                    snprintf(state.text_buffer, ARR_SIZE(state.text_buffer),
                             "%s %s", path_file_name(f_hash_1->file_name),
                             path_file_name(f_hash_2->file_name));

                    if (nk_selectable_image_label(ctx, img, state.text_buffer,
                                                  NK_TEXT_CENTERED,
                                                  &(state.img_selected[i]))) {
                        memset(state.img_selected, 0,
                               sizeof(i32) * matches->len);
                        state.img_current = i;
                        state.img_selected[i] = 1;
                    }
                }

                nk_group_end(ctx);
            }

            nk_layout_row_push(ctx, 0.75f);

            if (nk_group_begin(ctx, "Visualization", 0)) {
                if (matches->len > 0) {
                    // Delete buttons
                    {
                        nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
                        nk_layout_row_push(ctx, 0.5f);
                        if (nk_button_label(ctx, "Delete")) {
                            file_hash *f_hash_1 =
                                &matches->data[state.img_current];
                            ui_on_click_delete(matches,
                                               (usize)state.img_current,
                                               f_hash_1->file_name);
                        }
                        if (nk_button_label(ctx, "Delete")) {
                            file_hash *f_hash_2 =
                                &matches->data[state.img_current + 1];
                            ui_on_click_delete(matches,
                                               (usize)state.img_current,
                                               f_hash_2->file_name);
                        }

                        nk_layout_row_end(ctx);
                    }
                    if (matches->len == 0) goto end;

                    i32 img_current_next = (state.img_current + 1);
                    // Images
                    {
                        nk_layout_row_begin(ctx, NK_DYNAMIC,
                                            window_height * 0.75f, 2);
                        nk_layout_row_push(ctx, 0.5f);

                        SDL_Surface *surface_current =
                            matches->data[state.img_current].h.img.surface_src;

                        struct nk_image img_a = {
                            .handle = {.id =
                                           (i32)texture_ids[state.img_current]},
                            .w = surface_current->w,
                            .h = surface_current->h,
                            .region = {0, 0, surface_current->w,
                                       surface_current->h}};
                        nk_image(ctx, img_a);

                        nk_layout_row_push(ctx, 0.5f);
                        {
                            SDL_Surface *next = matches->data[img_current_next]
                                                    .h.img.surface_src;
                            struct nk_image img_b = {
                                .handle = {.id = (i32)
                                               texture_ids[img_current_next]},
                                .w = next->w,
                                .h = next->h,
                                .region = {0, 0, next->w, next->h}};
                            nk_image(ctx, img_b);
                        }
                        nk_layout_row_end(ctx);
                    }

                    // File paths
                    {
                        nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
                        nk_layout_row_push(ctx, 0.5f);
                        nk_label(ctx,
                                 matches->data[state.img_current].file_name,
                                 NK_TEXT_ALIGN_CENTERED);
                        nk_label(ctx, matches->data[img_current_next].file_name,
                                 NK_TEXT_ALIGN_CENTERED);
                        nk_layout_row_end(ctx);
                    }
                }
                nk_group_end(ctx);
            }
        }
    end:
        nk_end(ctx);

        glViewport(0, 0, window_width, window_height);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY,
                      MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(window);
    }
}
