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
- Compiles and runs on Linux, macOS and Windows

### Not (yet) working

- CP/M emulation
- SSH and SMB support for N:
- SAM voice synthesizer
- MIDIMaze support
- Program recorder (tape) emulation
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
git clone https://github.com/a8jan/fujinet-pc.git

# enter build directory
cd fujinet-pc/build

# prepare build
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug

# run build
cmake --build .
```
#### Windows

To build on Windows use MSYS2/CLANG64 environment. Start **CLANG64** shell (clang64.exe).

```sh
# get the source code
git clone https://github.com/a8jan/fujinet-pc.git

# enter build directory
cd fujinet-pc/build

# prepare build
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug -G Ninja

# run build
cmake --build .
```

Note: It seems Ninja build does not work on mapped disk (network drive) due to UNC paths not supported by `cmd.exe`.

```sh
# To use Make (slower) instead of Ninja
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug -G "MSYS Makefiles"
```

## Run it

TODO: fix this section, run from other that `build` directory

```sh
# copy data files into build directory
cp -a ../data/* .

# optionally put some disk image(s) to sd directory
cp /your/dir/some/image.atr sd
```

```sh
# start fujinet with wrapper script
./run-fujinet
```

### Configure

Visit http://localhost:8000 and configure serial port or enable SIO over Network for communication with Altirra Atari emulator. For emulator option check for details [here](https://github.com/FujiNetWIFI/fujinet-emulator-bridge).

Connect SIO2PC/USB and boot the Atari from FujiNet.

By default fujinet web interface is available on port 8000 listening on all available IP addresses. This can be changed with `-u <URL>` parameter. For example:

```sh
# to limit the web interface only for machine which is running fujinet 
# and to listen on non-default port 9001
./run-fujinet -u http://localhost:9001

# "http://" part can be omitted, "0.0.0.0" indicates all available addresses
./run-fujinet -u 0.0.0.0:8080
```
