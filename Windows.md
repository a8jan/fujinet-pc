# Windows

Currently, on Windows it is possible to install WSL to get Ubuntu inside Windows. FujiNet-PC compiles and it can run in WSL Ubuntu.

**WIP warning:** FujiNet-PC does not compile natively on Windows yet. Don't use instructions below!

## MSYS2

Install and setup MSYS2

https://www.msys2.org/

### Install packages

Start MSYS2 MinGW 64-bit

```sh
$ pacman -S --needed base-devel mingw-w64-x86_64-toolchain
```

```sh
$ pacman -Su git
$ pacman -Su cmake
```

## TODO ...

```sh
# prepare build
cmake -G "MSYS Makefiles" .. -DCMAKE_BUILD_TYPE:STRING=Debug

# run build
cmake --build .
```