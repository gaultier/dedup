#pragma once

#include <inttypes.h>

#include "utils.h"
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "vendor/optparse.h"

typedef struct {
    bool verbose;
    const char* dir;
    bool windowless;
    usize size;
    u8 distance;
} options;

static void usage() {
    printf("\nUsage: dedup [OPTIONS] [PATH]\n");
    printf("Reports duplicate files in a fast and simple way.\n");
    printf(
        "\
Options:\n\
    -v --verbose                Verbose output\n\
    -h --help                   This help message\n\
    -s --size <size>            Maximum size of files to inspect\n\
    -W --windowless             Windowless\n\
    -d --distance               Hash distance for duplicate detection\n\
");
}

static void options_parse(char* argv[], options* opts) {
    // Defaults
    opts->verbose = false;
    opts->windowless = false;
    opts->distance = 4;
    opts->size = 100 * 1000 * 1000;  // 100 Mb

    // Parse
    struct optparse_long base_longopts[] = {
        {.longname = "help", .argtype = OPTPARSE_NONE, .shortname = 'h'},
        {.longname = "verbose", .argtype = OPTPARSE_NONE, .shortname = 'v'},
        {.longname = "size", .argtype = OPTPARSE_REQUIRED, .shortname = 's'},
        {.longname = "windowless", .argtype = OPTPARSE_NONE, .shortname = 'W'},
        {.longname = "distance",
         .argtype = OPTPARSE_REQUIRED,
         .shortname = 'd'},
        {0}};

    i32 help = false;
    i32 option;
    struct optparse options;

    optparse_init(&options, argv);
    while ((option = optparse_long(&options, base_longopts, NULL)) != -1) {
        switch (option) {
            case 'v':
                opts->verbose = true;
                break;
            case 'h':
                help = true;
                break;
            case 's':
                opts->size = strtoull(options.optarg, NULL, 10);
                break;
            case 'W':
                opts->windowless = true;
                break;
            case 'd':
                opts->distance = (u8)strtoul(options.optarg, NULL, 10);
                break;
            case '?':
            default:
                usage();
                exit(1);
                break;
        }
    }

    // Parse remaining arguments
    char* arg;
    if ((arg = optparse_arg(&options)))
        opts->dir = arg;
    else
        opts->dir = ".";  // FIXME: Windows

    // Show help and exit if requested
    if (help) {
        usage();
        exit(0);
    }
}

