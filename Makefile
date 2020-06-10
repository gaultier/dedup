.POSIX:

CFLAGS = -Wall -Wextra -Wpedantic -Wno-padded -g -std=c99 -isystem /usr/local/include -march=native
CFLAGS_RELEASE = -O2 -ffast-math
CFLAGS_DEBUG = -fsanitize=address
LDFLAGS = -flto
LDLIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread

.PHONY: clean all

C_FILES= main.c
H_FILES= $(wildcard *.h)

dedup_debug: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS) $(LDLIBS) $(C_FILES) -o $@

dedup_release: $(C_FILES) $(H_FILES)
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS) $(LDLIBS) $(C_FILES) -o $@

test: test.c
	$(CC) -std=c99 -g $(CFLAGS_DEBUG) $^ vendor/munit.c -o $@

all: dedup_release dedup_debug test

clean:
	rm -f *.o dedup_debug dedup_release
