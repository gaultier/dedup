#pragma once

#define DISTANCE_EPSILON (4)
#define MAX_FILES (1000 * 1000)
#define MAX_FILE_NAME_LEN (1024)
#define MAX_CHANNELS (64)
#undef DEBUG

#include <inttypes.h>
#include <stddef.h>
#include <sys/types.h>

typedef ssize_t isize;
typedef size_t usize;
typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef float f32;
typedef double f64;
