# Windows build environment setup

Windows native port is built with help of [MSYS2](https://www.msys2.org/) tools collection.

Alternatively, on Windows it is possible to install WSL to get Ubuntu inside Windows. FujiNet-PC compiles and it can run in WSL Ubuntu. There is nice [blog article](https://bocianu.atari.pl/blog/fujinetinaltirra) (google translate can help) prepared by *bocianu*.

## MSYS2

To install and setup MSYS2 follow the instructions on https://www.msys2.org/ ,no need to install `mingw-w64-x86_64-toolchain`.

For FujiNet-PC we need environment which uses UCRT runtime library (not old MSVCRT). Both `UCRT64` and `CLANG64` should work, `CLANG32` was not tested yet. More about MSYS2 environments [here](https://www.msys2.org/docs/environments/).


### Build tools

Start **MSYS2 MSYS** (window with `MSYS` in shell prompt) and install packages to make `CLANG64` build environment ready.

```sh
# install base-devel, if not yet installed
pacman -S --needed base-devel

# install CLANG64 toolchain
pacman -S --needed mingw-w64-clang-x86_64-toolchain

# install git, CLANG64 CMake and Ninja build system
pacman -S git mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-ninja
```
