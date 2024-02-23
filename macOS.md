# macOS build environment setup

## Homebrew

Detailed instructions can be found for example here: [How To Install and Use Homebrew on macOS](https://www.digitalocean.com/community/tutorials/how-to-install-and-use-homebrew-on-macos)

Quick steps below.

### Install Xcode Command Line Tools

From terminal:

```sh
xcode-select --install
```

### Install Homebrew

Download install script:

```sh
curl -fsSL -o install.sh https://raw.githubusercontent.com/Homebrew/install/master/install.sh
```

Check the downloaded `install.sh` script. If happy with it, run it:

```sh
/bin/bash install.sh
```

## Build tools

```sh
# install CMake
brew install cmake
```

CMake with all dependencies should be installed.

## FTDI VCP Drivers (optional)

To use SIO2USB cable or adapter based on Future Technologies chip you need to install MacOS driver for it:

https://ftdichip.com/Drivers/vcp-drivers/

VCP = virtual COM port

