# Image deduplicator

Detect duplicate images (by using an average image hash function) & files (using `murmur3`).

`dedup` recursively scans all files in the target directory. If a file is detected as a supported image format,
it reads its contents, and computes the average image hash. Otherwise it computes the murmur3 hash.
It then logs which files have the same hash.
An optional visualization is also available.

![Screenshot](https://github.com/gaultier/dedup/blob/master/screenshot.png)

## Quick start

```sh
# On macOS:
$ brew install sdl2 sdl2_image glew
# On Linux, see Dockerfile(s)

# Compile:
$ make all -j4

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

- [x] Add windows support (mingw?):
- [ ] Add dhash (gradient hash)
- [ ] Speed-up resize
- [ ] Add glob option
- [ ] CSV/JSON export option

## Docker (WIP)

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
- Development libraries installed for SDL, SDL_image, e.g in `~/Downloads`:

```sh
$ make dedup_release.exe -B LDFLAGS="-L ~/Downloads/SDL2-2.0.12/x86_64-w64-mingw32/lib/ -L ~/Downloads/glew-2.1.0/lib/Release/x64/ -L ~/Downloads/SDL2_image-2.0.5/x86_64-w64-mingw32/lib/" CC=x86_64-w64-mingw32-gcc

$ file dedup_release.exe
dedup_release.exe: PE32+ executable (console) x86-64, for MS Windows

$ make win64
```

References:
- https://www.libsdl.org/release/SDL2-2.0.12-win32-x64.zip
- https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.5-mingw.tar.gz


## LICENSE

BSD-3
