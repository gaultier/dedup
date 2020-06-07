#pragma once

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_surface.h>
#include <errno.h>

#include "config.h"
#include "image_resize.h"
#include "utils.h"
#include "window.h"

static u8* img_unpack_dbg(u8* img, usize width, usize height, u16 channels) {
    u8* out = pg_malloc(width * height * channels);

    for (usize i = 0; i < width * height; i++) {
        for (u16 c = 0; c < channels; c++) {
            out[channels * i + c] = img[i];
        }
    }

    return out;
}

static bool img_ahash(const char* file_name, const u8* file_content,
                      usize file_content_len, u64* hash,
                      SDL_Surface** surface_src,
                      SDL_Surface** surface_thumbnail,
                      SDL_Surface** surface_hash) {
    SDL_RWops* const rw_ops =
        SDL_RWFromConstMem(file_content, (i32)file_content_len);
    if (rw_ops == NULL) {
        LOG("%s:%d:SDL_RWFromConstMem failed for %s, skipping and resorting to "
            "hashing the content.\n",
            __FILE__, __LINE__, file_name);
        return false;
    }

    *surface_src = IMG_Load_RW(rw_ops, true);

    if (*surface_src == NULL) {
        LOG("Not an image %s, skipping and resorting to hashing the content.\n",
            file_name);
        return false;
    }

    if ((*surface_src)->pixels == NULL || (*surface_src)->w == 0 ||
        (*surface_src)->h == 0 || (*surface_src)->format->BytesPerPixel == 0) {
        LOG("%s:%d:Malformed image %s, skipping and resorting to hashing the "
            "content.\n",
            __FILE__, __LINE__, file_name);
        SDL_FreeSurface((*surface_src));
        return false;
    }

    const u16 img_channels = (*surface_src)->format->BytesPerPixel;
    u8 img_thumbnail[THUMBNAIL_W * THUMBNAIL_H] = {0};
    img_resize((*surface_src)->pixels, (usize)(*surface_src)->w,
               (usize)(*surface_src)->h, img_channels, img_thumbnail,
               THUMBNAIL_W, THUMBNAIL_H);

    if (!opts.windowless) {
        LOG("img_channels=%u\n", img_channels);
        u8* const dbg_img =
            img_unpack_dbg(img_thumbnail, THUMBNAIL_W, THUMBNAIL_H, 3);

        *surface_thumbnail = SDL_CreateRGBSurfaceWithFormat(
            0, THUMBNAIL_W, THUMBNAIL_H, 24, SDL_PIXELFORMAT_RGB24);
        if (*surface_thumbnail == NULL)
            DIE(EINVAL, "Creating surface failed: %s", SDL_GetError());

        memcpy((*surface_thumbnail)->pixels, dbg_img,
               THUMBNAIL_H * THUMBNAIL_W * 3);
    }

    const u8 thumbnail_avg_value =
        avg(img_thumbnail, THUMBNAIL_H * THUMBNAIL_W);

    // Pack an array of 64 bytes to an array of 64 bits, stored as an array
    // of 8 u8
    u8 img_thumbnail_threshold[8] = {0};
    for (u8 x = 0; x < THUMBNAIL_W; x++) {
        for (u8 y = 0; y < THUMBNAIL_H; y++) {
            img_thumbnail_threshold[y] +=
                (img_thumbnail[y * THUMBNAIL_W + x] > thumbnail_avg_value) << x;
        }
    }

    if (!opts.windowless) {
        LOG("file=%s thumbnail_avg_value=%u\n", file_name, thumbnail_avg_value);
        u8 img_thumbnail_threshold_unpacked[THUMBNAIL_W * THUMBNAIL_H] = {0};

        for (u8 x = 0; x < THUMBNAIL_W; x++) {
            for (u8 y = 0; y < THUMBNAIL_H; y++) {
                img_thumbnail_threshold_unpacked[y * THUMBNAIL_W + x] =
                    255 *
                    (img_thumbnail[y * THUMBNAIL_W + x] > thumbnail_avg_value);
            }
        }

        u8* const dbg_img = img_unpack_dbg(img_thumbnail_threshold_unpacked,

                                           THUMBNAIL_W, THUMBNAIL_H, 3);
        *surface_hash = SDL_CreateRGBSurfaceWithFormat(
            0, THUMBNAIL_W, THUMBNAIL_H, 24, SDL_PIXELFORMAT_RGB24);
        if (*surface_hash == NULL)
            DIE(EINVAL, "Creating surface failed: %s", SDL_GetError());

        memcpy((*surface_hash)->pixels, dbg_img, THUMBNAIL_H * THUMBNAIL_W * 3);
    }

    *hash = 0;
    for (u8 i = 0; i < 8; i++)
        *hash += (u64)img_thumbnail_threshold[i] << (i * 8);

    return true;
}

