
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#pragma once
#define GL_SILENCE_DEPRECATION 1
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include "file_hash.h"

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

static void ui_init(SDL_Window **window) {
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
}

static void ui_run(SDL_Window *window, file_hashes_buffer *matches) {
    pg_assert_uint64(matches->len, >=, 2);

    u32 texture_id;
    SDL_Surface *s = ui_texture_load(matches->data[0].file_name,
                                     matches->data[1].file_name, &texture_id);

    for (;;) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        if (event.type == SDL_QUIT) return;
    }
}
