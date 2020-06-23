
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

static MunitResult test_file_move_to_trash(const MunitParameter params[],
                                           void* data) {
    (void)params;
    (void)data;

    char path[100] = "/tmp/test_file_move_to_trash.XXXXXX";
    munit_assert_ptr(mktemp(path), !=, NULL);

    const char* file_name = path_file_name(path);

    munit_assert_memory_equal(23, file_name, "test_file_move_to_trash");

    munit_assert_int(file_move_to_trash(path), ==, 0);

    FILE* file = fopen(path, "r");
    const int err = errno;

    munit_assert_ptr(file, ==, NULL);
    munit_assert_int(err, ==, ENOENT);

    char trash[4000] = "0";
    usize trash_len = 0;
    path_trash(trash, &trash_len, ARR_SIZE(trash));

    char path_in_trash[4000] = "0";
    snprintf(path_in_trash, ARR_SIZE(path_in_trash), "%s/%s", trash, file_name);

    file = fopen(path_in_trash, "r");
    munit_assert_ptr(file, !=, NULL);

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
    {
        .name = "file_move_to_trash",
        .test = test_file_move_to_trash,
    },
    {0},
};

static const MunitSuite test_suite = {
    .tests = test_suite_tests,
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    return munit_suite_main(&test_suite, (void*)"µnit", argc, argv);
}
