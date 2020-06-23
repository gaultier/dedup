# Image deduplicator

Detect duplicate images (by using an average image hash function) & files (using `murmur3`).

`dedup` recursively scans all files in the target directory. If a file is detected as a supported image format,
it reads its contents, and computes the average image hash. Otherwise it computes the murmur3 hash.
It then logs which files have the same hash.
An optional visualization is also available.

## Quick start

```sh
# On macOS:
$ brew install sdl2 sdl2_image

# On Debian (development libraries):
$ apt install -y libsdl2-dev libsdl2-image-dev

# On Debian (runtime libraries):
$ apt install -y libsdl2-2.0.0 libsdl2-image-2.0.0

# Compile:
$ make all -j4

# But actually `make` is not even needed, we can alternatively just do (on Linux, adapt the `-gl` part for other platforms):
$ cc main.c -std=c99 -lSDL2 -lSDL2_image -lgl -o dedup_release

# See help
$ ./dedup_release --help

# Scan files under 100Mb
$ ./dedup_release /path/to/image/directory --size 100000000

# Scan files in the current directory and don't display a window for visualization
$ ./dedup_release --windowless

# Scan files in the current directory and be very loose in the duplicate detection (high distance)
$ ./dedup_release --distance 10

# Run the tests:
$ ./test
```

## Image support

TGA, BMP, PNM, XPM, XCF, PCX, GIF, JPG, TIF, LBM, PNG.

## Roadmap

- [ ] Add windows support (mingw?):
- [ ] Add dhash (gradient hash)
- [ ] Speed-up resize
- [ ] Add glob option
- [ ] CSV/JSON export option


## Docker

```sh
# Ubuntu:
$ docker build -t dedup -f Dockerfile .

# Alpine:
$ docker build -t dedup -f Dockerfile.alpine .

# Run it:
$ docker run -it dedup
```

## Configuration

A few limits are defined in `config.h`. Once they have been edited, recompile and rerun `dedup`.

## Cross-compiling for windows with mingw

Prerequisites:
- Mingw installed
- Development libraries installed for SDL, SDL_image, e.g in `/tmp/x86_64-w64-mingw32/lib/`

```sh
$ make -j4 -B dedup_release CC=x86_64-w64-mingw32-gcc LDFLAGS="-L /tmp/x86_64-w64-mingw32/lib/"
$ file dedup_release.exe
dedup_release.exe: PE32+ executable (console) x86-64, for MS Windows
```

References:
- https://www.libsdl.org/release/SDL2-2.0.12-win32-x64.zip
- https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-mingw.tar.gz


## LICENSE

BSD-3
