# FsWizard

## Intro

A Unix V6++ diskfile program.

Expected features:

- [x] Unix V6++ disk formatting and operations.
- [x] Unix V6++ command line interface.
- [ ] Transforming between Unix V6++ and other disk formats (e.g. VHDx).

## Building

This project is built using MinGW GCC 12.2.0 under Windows 11. Other versions of GCC or Clang compilers might work, but the language support for C++17 is mandatory.

### Using CMake

```bash
$ mkdir build
$ cd build -G "MinGW Makefiles"
$ cmake .. 
$ make
```

### Using Makefile

```bash
$ mkdir build
$ make all
```

## How to use

To create a Unix V6++ image, the bootloader file, kernel file and all diskfiles must be prepared. This is normally done by building the Unix V6++ system and copying these generated files to a reachable local path.

Suppose your files are in the directory `./etc`, and the `makeimage` program is in the directory `./build`:

```yaml
fswizard:
  build:
    makeimage.exe:
  etc:
    boot.bin: "bootloader file"
    kernel.bin: "kernel file"
    rootfs: "root file system"
      bin:
        ...:
      demos:
        ...:
      Shell.exe:
```

Then run the following commands and the generated image shall be at `./etc/c.img`:

```bash
$ cd build
$ makeimage.exe -image ../etc/c.img -kernel ../etc/kernel.bin -boot ../etc/boot.bin -rootfs ../etc/rootfs
```

Once a disk image is generated, you can operate the image using the file system client program:

```bash
$ v6pp-fs-cli.exe -image ../etc/c.img
```

## Courtesy

- [Dr. Rong Deng](https://github.com/Deng-Rong) for OS course and reference materials.
- [Tianyao Gong](https://github.com/FlowerBlackG) who developed the [V6++ disk editor](https://github.com/FlowerBlackG/unix-v6pp-filesystem-editor) for my shameless reference. (LOL)

## Disclaimer

The source code of this repository can only be used for personal learning purposes. The source code and its creators are not responsible for the use of the source code. Any form of improper use is at your own peril.

For more details view the LICENSE file.
