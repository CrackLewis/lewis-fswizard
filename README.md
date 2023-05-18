# FsWizard

## Intro

A Unix V6++ diskfile program.

Expected features:

- [x] Unix V6++ disk formatting and operations.
- [x] Unix V6++ command line interface.
- [ ] Transforming between Unix V6++ and other disk formats (e.g. VHDx).
- [ ] Unix V6++ network application.

## Building

This project is built using MinGW GCC 12.2.0 under Windows 11. Other versions of GCC or Clang compilers might work, but the language support for C++17 and POSIX thread support is mandatory.

You can obtain a copy of the compiler I use through the link: [Compiler Download](https://github.com/niXman/mingw-builds-binaries/releases/download/12.2.0-rt_v10-rev2/x86_64-12.2.0-release-posix-seh-msvcrt-rt_v10-rev2.7z). 

If there're multiple compilers in your computer, you might have to specify the compiler toolchain path in the `CMakeLists.txt` or `Makefile`. 

### Using CMake

```bash
$ mkdir build
$ cd build 
$ cmake .. -G "MinGW Makefiles"
$ make
```

### Using Makefile

```bash
$ mkdir build
$ make all CC=/path/to/gcc
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
$ v6pp-fs-local.exe -image ../etc/c.img
```

The client program not only supports a variety of basic Unix file utilities, but it also allows you to read disk data by using `testblock <block_id>`.

## Courtesy

- [Dr. Rong Deng](https://github.com/Deng-Rong) for OS course and reference materials.
- [Tianyao Gong](https://github.com/FlowerBlackG) who developed the [V6++ disk editor](https://github.com/FlowerBlackG/unix-v6pp-filesystem-editor) for my shameless reference. (LOL)

## Disclaimer

The source code of this repository can only be used for personal learning purposes. The creator of the source code in this repository is not responsible for the use of the source code. Any form of improper use is at your own peril.

For more details view the LICENSE file.
