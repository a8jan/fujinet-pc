FujiNet-PC   
===========

FujiNet-PC is port of [FujiNet](https://github.com/FujiNetWIFI) firmware for Linux, macOS and Windows.

FujiNet is a multi-peripheral emulator and WiFi network device for vintage computers - https://fujinet.online.

FujiNet project on GitHub https://github.com/FujiNetWIFI

-------------------------------------------------------------------

<div style="text-align: center;">
<p><img src="merge.png" alt="Merge Sign"></img></p>
<p style="font-size: large;">We merged!</p>
<p>FujiNet-PC code is now part of <a href="https://github.com/FujiNetWIFI/fujinet-firmware">FujiNet firmware repository</a>.</p>
</div>  


-------------------------------------------------------------------

## Port Status

### Targets

FujiNet-PC is available for these target platforms:

- Atari 8-bit
- Apple II

More targets covered by the parent FujiNet project will be added in a future.

### Working

#### All targets

- FujiNet **network device** with support for various network protocols:
  - TCP, UDP, TNFS, HTTP(s), SMB, FTP, Telnet, SSH
- Data parsers:
  - JSON and XML
- Hash and encode/decode functions:
  - SHA-1, SHA-256, SHA-512, Base64
- TNFS, SMB and NFS file system to access image files over network
- Web interface to control program's settings, browse and mount disk images

#### Atari 8-bit

- Disk drive (D:) emulation with support for ATR disk images and XEX files (no ATX yet)
- Modem emulation (R:)
- Printer emulation (P:)
- APETIME
- PCLink
- CP/M emulation
- Can connect to Atari via SIO2PC/USB cable
- Can connect to [Altirra Atari emulator](https://virtualdub.org/altirra.html) with help of [emulator bridge](https://github.com/FujiNetWIFI/fujinet-emulator-bridge)<br>
  It is possible to run Altirra on Linux via Wine or on Mac via Crossover.

#### Apple II

- Disk drive emulation with support for PO, DSK, WOZ and HDV disk images
- More to be added ...
- Can connect to [updated AppleWin](https://github.com/FujiNetWIFI/AppleWin) (Windows, Linux) Apple II emulator<br>
  It is possible to run AppleWin on Mac via Crossover.

### Not (yet) working

- SAM voice synthesizer
- MIDIMaze support
- Cassette player/recorder
- ATX disk images

-------------------------------------------------------------------

## Download

FujiNet-PC pre-compiled binaries for few systems are available in [Releases](https://github.com/FujiNetWIFI/fujinet-pc/releases) section.

Do you want to try Altirra or AppleWin together with FujiNet-PC? There is a [FujiNet virtual appliance!](https://github.com/FujiNetWIFI/fujinet-vm)<br>
It can be downloaded [here (from MEGA)](http://go.atariorbit.org/virtual) and detailed documentation is available [here](https://fujinet-vm.readthedocs.io/). 

If interested into running FujiNet-PC with Altirra take a look at [FujiNet-PC Launcher](https://github.com/a8jan/fujinet-pc-launcher), bundle with GUI or lightweight scripts package can be an option for you.

To run FujiNet-PC with AppleWin emulator please follow ... *To Be Added*

## Building

The build process is controlled with [CMake](https://cmake.org/). Build works with GNU Make or Ninja build systems, with GCC or Clang/LLVM C and C++ compilers.

### Build environment

Prepare the build environment, there are instructions for **macOS [here](macOS.md)** and for **Windows [here](Windows.md)**.

To install necessary build tools on **Debian**, **Ubuntu** and derivatives:

```sh
sudo apt install cmake g++
```

### Dependencies

Install necessary libraries.

#### Debian/Ubuntu

```sh
sudo apt install libexpat-dev libmbedtls-dev
```

#### macOS

```sh
brew install mbedtls
```

#### Windows MSYS2 CLANG64

```sh
pacman -S mingw-w64-clang-x86_64-mbedtls
```

#### Python packages (all platforms)

Install Python packages for scripts used for build.

```sh
python -m pip install -U Jinja2 pyyaml
```

### Build

To build on Windows use MSYS2/CLANG64 environment. Start **CLANG64** shell (clang64.exe). On Linux and macOS use your favorite shell.

```sh
# get the source code
git clone https://github.com/FujiNetWIFI/fujinet-firmware.git

# enter source code directory
cd fujinet-firmware

# run the build script
build.sh -p ATARI # or -p APPLE

```

The result of successful build is in **`build/dist`** directory.

## SD Card

FujiNet-PC uses SD folder, not real SD Card. Visit [FujiNet SD Card](https://github.com/FujiNetWIFI/fujinet-sd-card) repository to get some useful utilities which can be placed into SD folder.


## Run it

Directory `build/dist` contains files needed to run FujiNet-PC. You can run FujiNet-PC directly inside `dist` or copy/move/rename the `dist` directory to the place of your preference and run it from there.

```sh
# enter dist directory (or copied/moved/renamed directory, if you copied/moved/...), must be inside
cd build/dist

# optionally, put some additional disk image(s) to SD sub-directory
cp /your/dir/some/image.atr SD

# start fujinet with "runner" helper script
./run-fujinet  # or run-fujinet.bat or run-fujinet.ps1 on Windows
```

It can be stopped with `Ctrl`+`C`

### Configure

Visit http://localhost:8000 (see below how to use different port for WebUI).

If connecting with real Atari configure Serial Port to match your SIO2PC/USB cable and disable SIO over Network in Emulator section. Connect SIO2PC/USB and boot the Atari from FujiNet.

For emulator options check instructions [here](https://github.com/FujiNetWIFI/fujinet-emulator-bridge). Alternatively, [FujiNet-PC Launcher](https://github.com/a8jan/fujinet-pc-launcher) can be used.

By default FujiNet Web Interface is available on port 8000 listening on all available IP addresses. This can be changed with `-u <URL>` parameter. For example:

```sh
# to limit the web interface only for machine which is running fujinet 
# and to listen on non-default port 8001
./run-fujinet -u http://localhost:8001

# "http://" part can be omitted
# this will make web interface available on any address assigned to PC/Mac/RPi
# port for web interface will be 8080
./run-fujinet -u 0.0.0.0:8080
```

## Additional information

Visit [FujiNet WiKi](https://github.com/FujiNetWIFI/fujinet-firmware/wiki) for load of useful information.

Join the [group on Discord](https://discord.gg/7MfFTvD).

Enjoy!
