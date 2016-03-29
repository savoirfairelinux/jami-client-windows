# Windows client for Ring

For more information about the ring project, see the following:

- Main website: https://ring.cx/
- Bug tracker: https://tuleap.ring.cx/projects/ring/
- Repositories: https://gerrit-ring.savoirfairelinux.com

## Requirements

- Ring client library and Ring daemon
- Mingw-w64 build environment
- Qt5 (we link against Qt5Core, Qt5Widgets, Qt5Gui) built with Mingw
- Qt5 Svg, Qt5 ImageFormats & Qt5 WinExtras built with Mingw

## Get the source code

 - ```git clone https://gerrit-ring.savoirfairelinux.com/ring-client-windows```

## Build instructions

*USE THE QMAKE OF Qt BUILT WITH MINGW* (e.g. /usr/bin/[i686 | x86_64]-w64-mingw32-qmake-qt5)

```
cd ring-client-windows
mkdir build && cd build
export QTDIR=<path to mingw qt> (e.g. /usr/[i686 | x86_64]-w64-mingw32/lib/qt)
git submodule init && git submodule update
cd libqrencode
./autogen.sh && ./configure --host=[i686 | x86_64]-w64-mingw32 --prefix=<Install dir of Ring and LRC>
make
make install
cd ..
qmake ../RingWinClient.pro -r -spec win32-g++ RING=<Install dir of Ring and LRC> [BUILD=Debug]
make
make install
```
You will find all files in ./release directory.

#Auto update

If you want to enable auto-update
- Build winsparkle
    In the client directory after having initialized the submodule
    ```
    cd winsparkle
    mkdir build && cd build
    cmake -DCMAKE_TOOLCHAIN_FILE=<LRC Directory>/cmake/winBuild.cmake -DCMAKE_INSTALL_PREFIX=Install dir of Ring and LRC> ../cmake",
    make
    make install
    ```
- Compile the client with `ENABLE_AUTOUPDATE=True`

## Debugging

Compile the client with 'BUILD=Debug' and libRingClient with '-DCMAKE_BUILD_TYPE=Debug'


## Packaging

* Nsis : Nullsoft Scriptable Install System :http://nsis.sourceforge.net/Main_Page.

```
cd build/release
makensis ring.nsi
```
