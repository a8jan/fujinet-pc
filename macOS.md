# macOS

## Get macOS

### KVM

Note: macOS inside KVM is slow (especially GUI), but for small project building over SSH it works fine.

Instructions can be found for example here: [How To run macOS on KVM / QEMU](https://computingforgeeks.com/how-to-run-macos-on-kvm-qemu/) 

```sh
git clone https://github.com/foxlet/macOS-Simple-KVM.git
```

Check `README.md` inside `macOS-Simple-KVM` directory.

### Install OS updates

## Homebrew

Instructions can be found for example here: [How To Install and Use Homebrew on macOS](https://www.digitalocean.com/community/tutorials/how-to-install-and-use-homebrew-on-macos)

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

Check the downloaded `install.sh` script. Run it:

```sh
/bin/bash install.sh
```

## Install build tools

### cmake

```sh
brew install cmake
```

cmake together with its dependencies should be installed

## Enable SSH

Optionally, to enable SSH navigate to System Preferences > Sharing. Enable Remote Login.

## FTDI VCP Drivers

Install MacOS driver for Future Technologies USB Serial chips, if using FT based cable/adapter:

https://ftdichip.com/Drivers/vcp-drivers/

VCP = virtual COM port

