#include "image_resize.h"
#include "utils.h"
#include "vendor/munit.h"

#define COLOR_BLACK (0, 0, 0)
#define COLOR_WHITE (255, 255, 255)
#define COLOR_RED (255, 0, 0)

static MunitResult test_img_resize_black(const MunitParameter params[],
                                         void* data) {
    (void)params;
    (void)data;

    static const u8 image_src_black[3 * 10 * 10] = {0};
    static const u8 image_thumbnail_black[THUMBNAIL_W * THUMBNAIL_H] = {0};

    u8 thumbnail[ARR_SIZE(image_thumbnail_black)] = {42, 42, 42};
    img_resize(image_src_black, 10, 10, 3, thumbnail, THUMBNAIL_W, THUMBNAIL_H);
    munit_assert_memory_equal(ARR_SIZE(thumbnail), thumbnail,
                              image_thumbnail_black);

    return MUNIT_OK;
}

static MunitResult test_img_resize_red(const MunitParameter params[],
                                       void* data) {
    (void)params;
    (void)data;

    static const u8 image_src_red[3 * 10 * 10] = {
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
    };
    static const u8 image_thumbnail_red[THUMBNAIL_W * THUMBNAIL_H] = {
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
    };

    u8 thumbnail[ARR_SIZE(image_thumbnail_red)] = {42, 42, 42};
    img_resize(image_src_red, 10, 10, 3, thumbnail, THUMBNAIL_W, THUMBNAIL_H);
    munit_assert_memory_equal(ARR_SIZE(thumbnail), thumbnail,
                              image_thumbnail_red);

    return MUNIT_OK;
}

static MunitResult test_img_resize_red_2x(const MunitParameter params[],
                                          void* data) {
    (void)params;
    (void)data;

    static const u8 image_src_red[3 * 20 * 20] = {
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0,
        255, 0, 0, 255, 0, 0,
    };
    static const u8 image_thumbnail_red[THUMBNAIL_W * THUMBNAIL_H] = {
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
        255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3, 255 / 3,
    };

    u8 thumbnail[ARR_SIZE(image_thumbnail_red)] = {42, 42, 42};
    img_resize(image_src_red, 10, 10, 3, thumbnail, THUMBNAIL_W, THUMBNAIL_H);
    munit_assert_memory_equal(ARR_SIZE(thumbnail), thumbnail,
                              image_thumbnail_red);

    return MUNIT_OK;
}

static MunitResult test_big_image(const MunitParameter params[], void* data) {
    (void)params;
    (void)data;

    /* u8* img; */
    /* usize img_len, img_width, img_height, img_data_size, img_data_offset; */
    /* bmp_load("/Users/pgaultier/Downloads/wallpaper.bmp", &img, &img_len, */
    /*          &img_width, &img_height, &img_data_size, &img_data_offset); */
    /* img += img_data_offset; */

    /* u8* thumbnail_expected; */
    /* usize thumbnail_expected_len, thumbnail_expected_width, */
    /*     thumbnail_expected_height, thumbnail_expected_data_size, */
    /*     thumbnail_expected_data_offset; */
    /* bmp_load("/Users/pgaultier/Downloads/wallpaper_thumbnail.bmp", */
    /*          &thumbnail_expected, &thumbnail_expected_len, */
    /*          &thumbnail_expected_width, &thumbnail_expected_height, */
    /*          &thumbnail_expected_data_size, &thumbnail_expected_data_offset);
     */
    /* thumbnail_expected += thumbnail_expected_data_offset; */

    /* u8 thumbnail[THUMBNAIL_H * THUMBNAIL_W] = {42, 42, 42}; */
    /* img_resize(img, img_width, img_height, 3, thumbnail, THUMBNAIL_W, */
    /*            THUMBNAIL_H); */

    /* for (usize i = 0; i < THUMBNAIL_W * THUMBNAIL_H; i++) { */
    /*     munit_assert_uint8(thumbnail[i], ==, thumbnail_expected[i * 3]); */
    /* } */

    return MUNIT_OK;
}

static MunitTest test_suite_tests[] = {
    {
        .name = "img_resize_black",
        .test = test_img_resize_black,
    },
    {
        .name = "img_resize_red",
        .test = test_img_resize_red,
    },
    {
        .name = "img_resize_red_2x",
        .test = test_img_resize_red_2x,
    },
    {0},
};

static const MunitSuite test_suite = {
    .tests = test_suite_tests,
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    return munit_suite_main(&test_suite, (void*)"µnit", argc, argv);
}
