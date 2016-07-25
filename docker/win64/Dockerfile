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
 mingw64-qt5-qtsvg \
 mingw64-qt5-qtwinextras


ENV CMAKE_PREFIX_PATH /usr/i686-w64-mingw32/sys-root/mingw/lib/cmake:/usr/x86_64-w64-mingw32/sys-root/mingw/lib/cmake
ENV QTDIR /usr/i686-w64-mingw32/sys-root/mingw/lib/qt5/:/usr/x86_64-w64-mingw32/sys-root/mingw/lib/qt5/
ENV PATH /usr/i686-w64-mingw32/bin/qt5/:/usr/x86_64-w64-mingw32/bin/qt5/:$PATH

#add user
RUN useradd jenkins -m
RUN echo -e "jenkins:jenkins" | chpasswd
RUN echo "jenkins ALL= NOPASSWD: ALL" >> /etc/sudoers

WORKDIR /home/jenkins

RUN ssh-keygen -b 1024 -f /etc/ssh/ssh_host_key -N ""
RUN ssh-keygen -f /etc/ssh/ssh_host_dsa_key -N  ""
RUN ssh-keygen -f /etc/ssh/ssh_host_rsa_key -N ""

RUN mkdir /home/jenkins/.ssh/
ADD known_hosts /home/jenkins/.ssh/
RUN chown jenkins:jenkins /home/jenkins/.ssh/*
RUN chown 600 /home/jenkins/.ssh/*

EXPOSE 22

USER root

#CMD ["/bin/bash"]
