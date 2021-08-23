FujiNet-PC   
=========

Work in progress [FujiNet firmware](https://github.com/FujiNetWIFI/fujinet-platformio) port to Linux, MacOS and Windows

#### If your are interested into FujiNet - A multi-function peripheral built on ESP32 hardware being developed for the Atari 8-bit systems - please visit the project at GitHub: https://github.com/FujiNetWIFI ####

-------------------------------------------------------------------

**Warning:** FujiNet-PC project is still in early development phase with ported components in various state of completeness and erroneousness...

-------------------------------------------------------------------

### Port Status

#### Working

- Disk drive (D:) emulation with support for ATR disk images and XEX files (no ATX yet)
- Modem emulation (R:)
- Printer emulation (P:)
- APETIME protocol
- TNFS File System to access image files over network
- Web interface to control program's settings, browse TNFS hosts and mount disk images
- Compiles and runs on Linux and macOS

#### Not (yet) working

- FujiNet network device (N:) with support for various network protocols
- CP/M emulation
- SAM voice synthesizer
- MIDIMaze support
- Windows port

-------------------------------------------------------------------

### Build instructions

```sh
# get source
git clone https://github.com/a8jan/fujinet-pc.git

# enter build directory
cd fujinet-pc/build

# prepare build
cmake .. -DCMAKE_BUILD_TYPE:STRING=Debug

# run build
cmake --build .

# copy data files into build directory
cp -a ../data/* .

# create directory simulating SD card
mkdir sd # lowercase sd

# and put some disk image(s) to sd directory (optional)
cp /your/dir/some/image.atr sd

# start fujinet
./run-fujinet

# visit http://localhost:8000 and configure serial port


# connect SIO2PC/USB and boot the Atari from FujiNet
```

By default fujinet web interface is available on port 8000 listening on all available IP addresses. This can be changed with `-u <URL>` parameter. For example:

```sh
# to limit the web interface only for machine which is running fujinet and to listen on non-default port 9001
./run-fujinet -u http://localhost:9001

# "http://" part can be omitted, "0.0.0.0" indicates all available addresses
./run-fujinet -u 0.0.0.0:8080
```
