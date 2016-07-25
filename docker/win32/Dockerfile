FROM fedora:23

RUN dnf -y upgrade

# Mingw
RUN dnf -y install mingw32-binutils \
 mingw32-gcc \
 mingw32-headers \
 mingw32-crt \
 mingw32-gcc-c++ \
 mingw32-pkg-config \
 mingw64-binutils \
 mingw64-gcc \
 mingw64-headers \
 mingw64-crt \
 mingw64-gcc-c++ \
 mingw64-pkg-config

# Daemon Req
RUN dnf -y install git \
 yasm \
 gettext-devel \
 cmake \
 patch \
 libtool \
 automake \
 make \
 xz \
 bzip2 \
 which

# LRC Req
RUN dnf -y install mingw32-qt5-qtbase \
 mingw32-qt5-qttools \
 mingw64-qt5-qtbase \
 mingw64-qt5-qttools

# Client Req
RUN dnf -y install mingw32-qt5-qtsvg \
 mingw32-qt5-qtwinextras \
 mingw32-qt5-imageformats \
 mingw64-qt5-qtsvg \
 mingw64-qt5-qtwinextras \
 mingw64-qt5-imageformats

ENV CMAKE_PREFIX_PATH /usr/i686-w64-mingw32/sys-root/mingw/lib/cmake:/usr/x86_64-w64-mingw32/sys-root/mingw/lib/cmake
ENV QTDIR /usr/i686-w64-mingw32/sys-root/mingw/lib/qt5/:/usr/x86_64-w64-mingw32/sys-root/mingw/lib/qt5/
ENV PATH /usr/i686-w64-mingw32/bin/qt5/:/usr/x86_64-w64-mingw32/bin/qt5/:$PATH

#Install packager
dnf -y install  mingw32-nsis

#add user
RUN useradd joulupukki -m
RUN echo "joulupukki ALL= NOPASSWD: ALL" >> /etc/sudoers

USER joulupukki
WORKDIR /home/joulupukki
