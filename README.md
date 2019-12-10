# Jami-qt

`jami-qt` is the cross platform client for Jami. For now, it's mainly used for the Windows platform and is not tested on other platforms.

![jami-logo](images/logo-jami-standard-coul.png)


For more information about the jami project, see the following:

- Main website: https://jami.net/
- Bug tracker: https://git.jami.net/
- Repositories: https://gerrit-ring.savoirfairelinux.com

## Requirements

- Jami client library and Jami daemon
- Mingw-w64 build environment
- Qt5 (we link against Qt5Core, Qt5Widgets, Qt5Gui) built with Mingw
- Qt5 Svg, Qt5 ImageFormats & Qt5 WinExtras built with Mingw

## Get the source code

 - `git clone https://gerrit-ring.savoirfairelinux.com/ring-client-windows`

## Build instructions

**NOTE: The build process is currently under a full refactorization. This section will be updated soon**

## Windows

TBD

### Packaging

* Nsis : Nullsoft Scriptable Install System :http://nsis.sourceforge.net/Main_Page.

```
cd build/release
makensis ring.nsi
```


## Linux

Note: LRC should be compiled or installed on your system.

- For Debian based:
```bash
    sudo apt install qtmultimedia5-dev libqt5svg5* qtwebengine5-dev qtdeclarative5-dev pandoc
```
- For Fedora:
```bash
    sudo dnf install qt5-qtsvg-devel qt5-qtwebengine-devel qt5-multimedia-devel qt5-qtdeclarative-devel pandoc
```

**Build Windows Client**

```bash
    cd ring-client-windows
    pandoc -f markdown -t html5 -o changelog.html changelog.md
    mkdir build
    cd build
    qmake -qt=qt5 ../jami-qt.pro
    make -j9
```
- Then, you are finally ready to launch jami-qt in your build directory.

#### Debugging

Compile the client with `BUILD=Debug` and compile LibRingClient with
`-DCMAKE_BUILD_TYPE=Debug`

## Mac OS

TBD


## Debugging

Compile the client with 'BUILD=Debug' and libRingClient with '-DCMAKE_BUILD_TYPE=Debug'