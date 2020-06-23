.POSIX:

CFLAGS = -Wall -Wextra -Wpedantic -Wno-padded -g -std=c99 -isystem /usr/local/include
CFLAGS_RELEASE = -O2 -ffast-math -march=native
CFLAGS_DEBUG = -fsanitize=address
LDFLAGS = -flto
LDLIBS = -lSDL2 -lSDL2_image -lpthread -lglew

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDLIBS += -lGL
endif
ifeq ($(UNAME_S),Darwin)
    LDLIBS += -framework OpenGL
endif

.PHONY: clean all win64 macos

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

win64: dedup_release.exe
	rm -rf dedup-win64
	mkdir dedup-win64
	cp dedup_release.exe dedup-win64/dedup.exe
	cp libs-w64/*.dll dedup-win64
	zip -r dedup-win64.zip dedup-win64

macos: dedup_release
	rm -rf dedup-macos
	mkdir dedup-macos
	cp dedup_release dedup-macos/dedup
	cp /usr/local/lib/libSDL2.dylib /usr/local/lib/libSDL2_image.dylib /usr/local/lib/libGLEW.dylib dedup-macos
	zip -r dedup-macos.zip dedup-macos


clean:
	rm -rf *.o dedup_debug dedup_release dedup-macos dedup-win64 *.zip
