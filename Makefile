.POSIX:

CFLAGS = -Wall -Wextra -Wpedantic -Wno-padded -g -std=c99 -isystem /usr/local/include
CFLAGS_RELEASE = -O2 -ffast-math -march=native
CFLAGS_DEBUG = -fsanitize=address
LDFLAGS = -flto
LDLIBS = -lSDL2 -lSDL2_image -lpthread -lglew

.PHONY: clean all

C_FILES= main.c
H_FILES= $(wildcard *.h)

dedup_debug: $(C_FILES) $(H_FILES)
	$(CC) $(C_FILES) $(CFLAGS) $(CFLAGS_DEBUG) $(LDFLAGS) $(LDLIBS) -o $@

dedup_release: $(C_FILES) $(H_FILES)
	$(CC) $(C_FILES) $(CFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS) $(LDLIBS) -o $@

test: test.c dedup_debug
	$(CC) -std=c99 -g $(CFLAGS_DEBUG) test.c vendor/munit.c -o $@

all: dedup_release dedup_debug test

dedup_release.exe: $(C_FILES) $(H_FILES)
	$(CC) main.c $(CFLAGS) $(CFLAGS_RELEASE) $(LDFLAGS) -lSDL2 -lSDL2_image -lglew32 -lopengl32 -o $@

clean:
	rm -f *.o dedup_debug dedup_release
