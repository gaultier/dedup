# Image deduplicator

Detect duplicate images (by using an average image hash function) & files (using `murmur3`).

`dedup` recursively scans all files in the target directory. If a file is detected as a supported image format,
it reads its contents, and computes the average image hash. Otherwise it computes the murmur3 hash.
It then logs which files have the same hash.
An optional visualization is also available.

## Quick start

```sh
# On macOS:
$ brew install sdl2 sdl2_image sdl2_ttf

# On Debian (development libraries):
$ apt install -y libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

# On Debian (runtime libraries):
$ apt install -y libsdl2-2.0.0 libsdl2-image-2.0.0 libsdl2-ttf-2.0-0

# Compile:
$ make all -j4

# But actually `make` is not even needed, we can alternatively just do:
$ cc main.c -std=c99 -lSDL2 -lSDL2_image -lSDL2_ttf -o dedup_release

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


## LICENSE

BSD-3
