FujiNet-PC   
=========

Work in progress [FujiNet firmware](https://github.com/FujiNetWIFI/fujinet-platformio) port to Linux, macOS and Windows

**If your are interested into FujiNet - A multi-function peripheral built on ESP32 hardware being developed for the Atari 8-bit systems - please visit the project at GitHub: https://github.com/FujiNetWIFI**

-------------------------------------------------------------------

**Warning:** FujiNet-PC is still work in progress with ported components in various state of completeness and erroneousness...

-------------------------------------------------------------------

## Port Status

### Working

- Disk drive (D:) emulation with support for ATR disk images and XEX files (no ATX yet)
- Modem emulation (R:)
- Printer emulation (P:)
- APETIME protocol
- TNFS File System to access image files over network
- Web interface to control program's settings, browse TNFS hosts and mount disk images
- FujiNet network device (N:) with support for various network protocols:
  TCP, UDP, TNFS, HTTP, FTP, Telnet
- Compiles and runs on Linux, macOS and Windows (Windows with Altirra only, no serial port yet)

### Not (yet) working

- CP/M emulation
- SSH and SMB support for N:
- SAM voice synthesizer
- MIDIMaze support
- Program recorder (tape) emulation
- UART on Windows (to connect with Atari via SIO port)
- Distributable binary packages for supported platforms

-------------------------------------------------------------------

## Build instructions

The build process is controlled with [CMake](https://cmake.org/). Builds should work with GNU Make or Ninja build systems, with GCC or Clang/LLVM C and C++ compilers.

### Build tools

The steps to get ready for building on **macOS** are described [here](macOS.md) and for **Windows** [here](Windows.md).

To install necessary build tools on **Debian**, **Ubuntu** and derivatives:

```sh
sudo apt install cmake g++
```

### Dependencies

Install necessary build libraries.

#### Debian/Ubuntu

```sh
sudo apt install libexpat-dev libssl-dev
```

#### macOS

```sh
brew install openssl
```

### Build

#### Linux and macOS

```sh
# get the source code
git clone https://github.com/FujiNetWIFI/fujinet-pc.git

# enter build directory
cd fujinet-pc/build

# prepare build
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug

# run build
cmake --build .

# after successful build populate dist directory with necessary files
cmake --build . --target dist
```
#### Windows

To build on Windows use MSYS2/CLANG64 environment. Start **CLANG64** shell (clang64.exe).

Note: It seems build does not work on mapped disk (network drive with drive letter assigned) due to UNC paths not supported by `cmd.exe`. As a workaround the build can be run within build directory on local disk from source code on mapped disk.

```sh
# get the source code
git clone https://github.com/FujiNetWIFI/fujinet-pc.git

# enter build directory
cd fujinet-pc/build

# prepare build
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug -G Ninja
# alternatively, to use Make instead of Ninja
# cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug -G "MSYS Makefiles"

# run build
cmake --build .

# after successful build populate dist directory with necessary files
cmake --build . --target dist
```

#### SD Card

Part of `dist` build is to download [FujiNet SD Card](https://github.com/FujiNetWIFI/fujinet-sd-card) files from GitHub into SD "card" folder (FN-PC uses SD folder not real SD card).


## Run it

`dist` directory (within build directory) contains files needed to run FujiNet-PC. You can run it directly inside `dist` or copy/move/rename the `dist` directory to the place of your preference and run it from there.

```sh
# enter dist directory (or copied/moved/renamed directory, if you copied/...)
cd dist

# optionally put some additional disk image(s) to SD sub-directory
cp /your/dir/some/image.atr SD

# start fujinet with wrapper script
./run-fujinet  # or run-fujinet.bat from Windows command prompt
```

It can be stopped with `Ctrl`+`C`

### Configure

Visit http://localhost:8000 and configure serial port or enable SIO over Network for communication with Altirra Atari emulator.

For emulator option check details [here](https://github.com/FujiNetWIFI/fujinet-emulator-bridge).

Connect SIO2PC/USB and boot the Atari from FujiNet.

By default fujinet web interface is available on port 8000 listening on all available IP addresses. This can be changed with `-u <URL>` parameter. For example:

```sh
# to limit the web interface only for machine which is running fujinet 
# and to listen on non-default port 9001
./run-fujinet -u http://localhost:9001

# "http://" part can be omitted
# this will make web interface available on any address assigned to PC/Mac/RPi
# port for web interface will be 8080
./run-fujinet -u 0.0.0.0:8080
```
