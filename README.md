FujiNet-PC   
=========

Work in progress [FujiNet firmware](https://github.com/FujiNetWIFI/fujinet-platformio) port to Linux, MacOS and Windows

#### If your are interested into Fujinet - A multi-function peripheral built on ESP32 hardware being developed for the Atari 8-bit systems - please visit the project at GitHub: https://github.com/FujiNetWIFI ####

-------------------------------------------------------------------

**Warning:** This project is still in early development phase with ported components in various state of completeness and erroneousness...

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
./fujinet

# visit http://localhost:8000 and configure serial port

# connect SIO2PC/USB and boot the Atari from FujiNet
```

