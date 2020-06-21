
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_video.h>
#pragma once
#define GL_SILENCE_DEPRECATION 1
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

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

static SDL_Surface *ui_texture_load(const char *path1, const char *path2,
                                    GLuint *texture_id) {
    SDL_Surface *img1 = IMG_Load(path1);
    pg_assert_ptr(img1, !=, NULL);

    SDL_Surface *img2 = IMG_Load(path2);
    pg_assert_ptr(img2, !=, NULL);

    pg_assert_int(img1->format->BytesPerPixel, ==, img2->format->BytesPerPixel);
    pg_assert_int(img1->w, ==, img2->w);
    pg_assert_int(img1->h, ==, img2->h);

    u8 *img_pixels =
        malloc((size_t)(img1->w * img1->h * img1->format->BytesPerPixel +
                        img2->w * img2->h * img2->format->BytesPerPixel +
                        img1->w * img2->format->BytesPerPixel));
    pg_assert_ptr(img_pixels, !=, NULL);

    memset(img_pixels, 255,
           img1->w * img1->h * img1->format->BytesPerPixel +
               img2->w * img2->h * img2->format->BytesPerPixel);
    memcpy(img_pixels, img1->pixels,
           img1->w * img1->h * img1->format->BytesPerPixel);
    memcpy(img_pixels + img1->w * img1->h * img1->format->BytesPerPixel +
               img1->w * img2->format->BytesPerPixel,
           img2->pixels, img2->w * img2->h * img2->format->BytesPerPixel);

    SDL_Surface *img = SDL_CreateRGBSurfaceWithFormatFrom(
        img_pixels, img1->w, img1->h + img2->h + 3, img1->format->BitsPerPixel,
        img1->w * img1->format->BytesPerPixel, img1->format->format);
    pg_assert_ptr(img, !=, NULL);

    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, img->pixels);
    return img;
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
                               SDL_WINDOWPOS_CENTERED, 800, 600,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
                                   SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!*window) DIE(EINVAL, "Could not create window: %s\n", SDL_GetError());

    SDL_GLContext *context = SDL_GL_CreateContext(*window);
    if (!context)
        DIE(EINVAL, "Could not initialize OpenGL: %s\n", SDL_GetError());

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

static void ui_run(SDL_Window *window, void *nuklear_ctx,
                   file_hashes_buffer *matches) {
    struct nk_context *ctx = nuklear_ctx;
    pg_assert_uint64(matches->len, >=, 2);

    u32 texture_id;
    SDL_Surface *s = ui_texture_load(matches->data[0].file_name,
                                     matches->data[1].file_name, &texture_id);

    int img_selected = 0;

    for (;;) {
        i32 window_width, window_height;
        SDL_GetWindowSize(window, &window_width, &window_height);

        SDL_Event event;
        nk_input_begin(ctx);
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT) return;
        nk_sdl_handle_event(&event);
        nk_input_end(ctx);

        if (nk_begin(ctx, "Image Deduper",
                     nk_rect(0, 0, window_width, window_height),
                     NK_WINDOW_BORDER)) {
            nk_layout_row_begin(ctx, NK_DYNAMIC, window_height, 2);
            nk_layout_row_push(ctx, 0.25f);
            if (nk_group_begin(ctx, "Preview", 0)) {
                nk_layout_row_dynamic(ctx, 0, 1);

                for (int i = 0; i < 2; i++) {
                    nk_layout_row_dynamic(ctx, 80, 1);

                    struct nk_image img = {.handle = (void *)texture_id,
                                           .w = s->w,
                                           .h = s->h,
                                           .region = {0, 0, s->w, s->h}};
                    if (nk_selectable_image_label(ctx, img, "An image",
                                                  NK_TEXT_CENTERED,
                                                  &img_selected)) {
                        /* memset(state->img_selected, 0, 2 * sizeof(int)); */
                        /* state->img_current = i; */
                        /* state->img_selected[i] = 1; */
                    }
                }
            }
            nk_group_end(ctx);
        }
        nk_end(ctx);

        glViewport(0, 0, window_width, window_height);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY,
                      MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(window);
    }
}
