# Windows

Currently, on Windows it is possible to install WSL to get Ubuntu inside Windows. FujiNet-PC compiles and it can run in WSL Ubuntu.

**WIP warning:** FujiNet-PC does not compile natively on Windows yet. Don't use instructions below!

## MSYS2

Install and setup MSYS2

https://www.msys2.org/

### Install packages

Start MSYS2

```sh
# TODO not sure if mingw-w64-x86_64-toolchain is necessary
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-toolchain
```

```sh
$ pacman -Su git
$ pacman -Su mingw-w64-x86_64-cmake
```

## TODO ... Build (WIP!)

```sh
# create build directory for Windows build
cd build
mkdir windows-x86_64 && cd windows-x86_64
```

```sh
# prepare build
#cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug -G "MSYS Makefiles"
/mingw64/bin/cmake ../.. -DCMAKE_BUILD_TYPE:STRING=Debug -G "MSYS Makefiles"

# run build
/mingw64/bin/cmake --build .
```