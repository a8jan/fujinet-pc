# Windows

## Get Windows

https://www.microsoft.com/en-us/windows/get-windows-10

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

