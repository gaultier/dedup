#pragma once
#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

#include "config.h"

#define pg_assert_type_full(prefix, suffix, T, fmt, a, op, b)                  \
    do {                                                                       \
        T pg_tmp_a_ = (a);                                                     \
        T pg_tmp_b_ = (b);                                                     \
        if (!(pg_tmp_a_ op pg_tmp_b_)) {                                       \
            fprintf(stderr,                                                    \
                    "%s:%d:assertion failed: %s %s %s (" prefix "%" fmt suffix \
                    " %s " prefix "%" fmt suffix ")\n",                        \
                    __FILE__, __LINE__, #a, #op, #b, pg_tmp_a_, #op,           \
                    pg_tmp_b_);                                                \
            abort();                                                           \
        }                                                                      \
    } while (0)

#define pg_assert_type(T, fmt, a, op, b) \
    pg_assert_type_full("", "", T, fmt, a, op, b)

#define pg_assert_char(a, op, b)           \
    pg_assert_type_full("'\\x", "'", char, \
                        "02"               \
                        "x",               \
                        a, op, b)
#define pg_assert_uchar(a, op, b)                   \
    pg_assert_type_full("'\\x", "'", unsigned char, \
                        "02"                        \
                        "x",                        \
                        a, op, b)
#define pg_assert_short(a, op, b) pg_assert_type(short, "d", a, op, b)
#define pg_assert_ushort(a, op, b) pg_assert_type(unsigned short, "u", a, op, b)
#define pg_assert_int(a, op, b) pg_assert_type(int, "d", a, op, b)
#define pg_assert_uint(a, op, b) pg_assert_type(unsigned int, "u", a, op, b)
#define pg_assert_long(a, op, b) pg_assert_type(long int, "ld", a, op, b)
#define pg_assert_ulong(a, op, b) \
    pg_assert_type(unsigned long int, "lu", a, op, b)
#define pg_assert_llong(a, op, b) pg_assert_type(long long int, "lld", a, op, b)
#define pg_assert_ullong(a, op, b) \
    pg_assert_type(unsigned long long int, "llu", a, op, b)

#define pg_assert_size(a, op, b) pg_assert_type(size_t, "u", a, op, b)

#define pg_assert_float(a, op, b) pg_assert_type(float, "f", a, op, b)
#define pg_assert_double(a, op, b) pg_assert_type(double, "g", a, op, b)
#define pg_assert_ptr(a, op, b) pg_assert_type(const void*, "p", a, op, b)

#define pg_assert_int8(a, op, b) pg_assert_type(int8_t, PRIi8, a, op, b)
#define pg_assert_uint8(a, op, b) pg_assert_type(uint8_t, PRIu8, a, op, b)
#define pg_assert_int16(a, op, b) pg_assert_type(int16_t, PRIi16, a, op, b)
#define pg_assert_uint16(a, op, b) pg_assert_type(uint16_t, PRIu16, a, op, b)
#define pg_assert_int32(a, op, b) pg_assert_type(int32_t, PRIi32, a, op, b)
#define pg_assert_uint32(a, op, b) pg_assert_type(uint32_t, PRIu32, a, op, b)
#define pg_assert_int64(a, op, b) pg_assert_type(int64_t, PRIi64, a, op, b)
#define pg_assert_uint64(a, op, b) pg_assert_type(uint64_t, PRIu64, a, op, b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define CLAMP(x, xmin, xmax) \
    ((x) < (xmin) ? (xmin) : (x) > (xmax) ? (xmax) : (x))

#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define UNREACHABLE()                                                       \
    do {                                                                    \
        fprintf(stderr,                                                     \
                "%s:%d:Reached unreachable code in function %s. This is a " \
                "bug in the compiler.\n",                                   \
                __FILE__, __LINE__, __func__);                              \
        abort();                                                            \
    } while (0)

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...)                                     \
    do {                                                        \
        printf("%s:%d: " fmt, __func__, __LINE__, __VA_ARGS__); \
    } while (0)
#else
#ifndef DEBUG
#define LOG_DEBUG(fmt, ...) \
    do {                    \
    } while (0)
#endif
#endif

static bool log_verbose = false;
#define LOG(fmt, ...)                                               \
    do {                                                            \
        if (log_verbose)                                            \
            printf("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define LOG_ERR(fmt, ...)                                                    \
    do {                                                                     \
        if (log_verbose)                                                     \
            fprintf(stderr, "%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#define DIE(err, fmt, ...)                                               \
    do {                                                                 \
        fprintf(stderr, "%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__); \
        exit(err);                                                       \
    } while (0)

static void* pg_malloc(usize size) {
    void* mem = malloc(size);
    if (!mem)
        DIE(ENOMEM, "Could not allocate %zu bytes, out-of-memory\n", size);

    LOG_DEBUG("%lu\n", size);

    return mem;
}

static void pg_free(void* ptr) { free(ptr); }

static u8 avg(u8* buffer, usize buffer_len) {
    usize value = 0;

    // TODO: could overflow
    for (usize i = 0; i < buffer_len; i++) value += buffer[i];

    value /= buffer_len;

    return (u8)value;
}

static u32 hamming_distance(u64 x, u64 y) {
    return (u32)__builtin_popcountll(x ^ y);
}

static u32 murmur_32_scramble(u32 k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}

static u32 murmur3_32(const u8* key, usize len, u32 seed) {
    u32 h = seed;
    u32 k;
    /* Read in groups of 4. */
    for (size_t i = len >> 2; i; i--) {
        // Here is a source of differing results across endiannesses.
        // A swap here has no effects on hash properties though.
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }
    /* Read the rest. */
    k = 0;
    for (usize i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    // A swap is *not* necessary here because the preceding loop already
    // places the low bytes in the low places according to whatever endianness
    // we use. Swaps only apply when the memory is copied in a chunk.
    h ^= murmur_32_scramble(k);
    /* Finalize. */
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static usize cores_count() {
#ifndef _WIN32
    return (usize)sysconf(_SC_NPROCESSORS_ONLN);
#else
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return (usize)sys_info.dwNumberOfProcessors;
#endif
}

static void print_humanize_usize(usize n, const char* unit) {
    if (n < 1024) {
        printf("%lu%s\n", n, unit);
    } else if (n < 1024 * 1024) {
        printf("%.1fK%s\n", n / 1024.0, unit);
    } else if (n < 1024 * 1024 * 1024) {
        printf("%.1fM%s\n", n / 1024.0 / 1024.0, unit);
    } else if (n < ((usize)1024.0 * 1024.0 * 1024.0 * 1024.0)) {
        printf("%.1fG%s\n", n / 1024.0 / 1024.0 / 1024.0, unit);
    } else if (n < ((usize)1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0)) {
        printf("%.1fT%s\n", n / 1024.0 / 1024.0 / 1024.0 / 1024.0, unit);
    } else {
        printf("%lu%s\n", n, unit);
    }
}

#ifndef _WIN32
static i32 file_info(const char* path, usize* size, bool* is_dir,
                     bool* is_regular_file) {
    struct stat st;
    if (stat(path, &st) != 0) {
        LOG_ERR("%s:%d:Could not stat `%s`: %s\n", __FILE__, __LINE__, path,
                strerror(errno));
        return errno;
    }

    if (size) *size = (usize)st.st_size;

    if (is_dir) *is_dir = S_ISDIR(st.st_mode);

    if (is_regular_file) *is_regular_file = S_ISREG(st.st_mode);
    return 0;
}
#else
static i32 file_info(const char* path, usize* size, bool* is_dir,
                     bool* is_regular_file) {
    struct _stat st;
    if (_stat(path, &st) != 0) {
        LOG_ERR("%s:%d:Could not stat `%s`: %s\n", __FILE__, __LINE__, path,
                strerror(errno));
        return errno;
    }

    if (size) *size = (usize)st.st_size;

    if (is_dir) *is_dir = st.st_mode & _S_IFDIR;

    if (is_regular_file) *is_regular_file = st.st_mode & _S_IFREG;
    return 0;
}
#endif
