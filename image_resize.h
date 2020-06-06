#pragma once

#include "utils.h"

#define THUMBNAIL_W 8
#define THUMBNAIL_H 8

static void img_resize(const u8* img_in, usize img_in_w, usize img_in_h,
                       u16 channels, u8* img_out, usize img_out_w,
                       usize img_out_h) {
    pg_assert_uint16(channels, <, MAX_CHANNELS);

    const f32 wsc = (f32)img_in_w / img_out_w;
    const f32 hsc = (f32)img_in_h / img_out_h;

    const u32 wsci = CLAMP(1, (u32)wsc, UINT32_MAX);  // Prevent size=0
    const u32 hsci = CLAMP(1, (u32)hsc, UINT32_MAX);  // Prevent size=0
    const u32 wh = wsci * hsci;

    // Each pixel in the `img_out` image is the average pixel inside a block in
    // the `img_in` image
    for (usize x = 0; x < img_out_w; x++) {
        for (usize y = 0; y < img_out_h; y++) {
            const usize src_x = (usize)(x * wsc);
            const usize src_y = (usize)(y * hsc);
            u32 chans[MAX_CHANNELS] = {0};

            for (usize xi = 0; xi < wsci; xi++) {
                for (usize yi = 0; yi < hsci; yi++) {
                    const usize i = src_x + xi + (src_y + yi) * img_in_w;

                    for (u16 c = 0; c < channels; c++) {
                        chans[c] += img_in[channels * i + c];
                    }
                }
            }
            // Average each channel for this pixel
            usize avg = 0;
            const usize i = x + y * img_out_w;
            for (u16 c = 0; c < channels; c++) {
                avg += chans[c] / wh;
            }
            avg /= channels;
            img_out[i] = (u8)avg;
        }
    }
}
