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

For now, this process is experimental. The best way to do that is:

1. Compile the daemon and LRC as specified in these projects (see the respective repositories or https://git.jami.net/savoirfairelinux/ring-project/wikis/technical/Build-instructions).
2. Install needed dependencies (TBD):
    + For Fedora:
```bash
sudo dnf install qt5-qtsvg-devel qt5-qtwebengine-devel qt5-multimedia-devel
```
3. If you are not using the `ring-project` repository, you have to define the `LRC` environment variable to contains the install directory for `LRC`. Also, you will have to setup `LD_LIBRARY_PATH` if your install directory is a custom one.
4. Then, build the client:

```bash
mkdir build
cd build
qmake-qt5 ../jami-qt.pro
make -j 9
```

5. Then, you are finally ready to launch `jami-qt` in your `build` directory.

### Known issues

1. The build system is not straight forward
2. Video doesn't work
3. Can't maximize/minimize window
4. Crash if the daemon is not started and installed.

## Mac OS

TBD


## Debugging

Compile the client with 'BUILD=Debug' and libRingClient with '-DCMAKE_BUILD_TYPE=Debug'