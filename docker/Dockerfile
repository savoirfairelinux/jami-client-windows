
FROM nfnty/arch-mini

#the ugliest hack on earth
VOLUME /tmp

#Update system and add archlinuxfr repo for yaourt
RUN pacman -Syu --noconfirm
RUN echo "[archlinuxfr]" >> /etc/pacman.conf
RUN echo "SigLevel = Never" >> /etc/pacman.conf
RUN echo "Server = http://repo.archlinux.fr/\$arch" >> /etc/pacman.conf
RUN cat /etc/pacman.conf

RUN pacman -Syu reflector rsync --noconfirm --needed
RUN reflector --verbose --country 'Canada' -l 200 --sort rate --save /etc/pacman.d/mirrorlist
RUN cat /etc/pacman.d/mirrorlist

#ensure that base and base-devel are installed and up to date
RUN pacman -Syu base base-devel --noconfirm --needed

#install some needed package
RUN pacman -Syu yaourt wget git yasm cmake --noconfirm --needed

#add user
RUN useradd joulupukki -m
RUN echo "joulupukki ALL= NOPASSWD: ALL" >> /etc/sudoers

USER joulupukki
WORKDIR /home/joulupukki

#init the key and add those used for aur packages we need
RUN sudo pacman-key --init && sudo pacman-key --populate archlinux
RUN gpg --keyserver hkp://keys.gnupg.net --recv-key D605848ED7E69871 9766E084FB0F43D8 4DE8FF2A63C7CC90 D9C4D26D0E604491 BB5869F064EA74AB

#install cross compil tool
RUN yaourt -S mingw-w64-toolchain mingw-w64-pkg-config --noconfirm --needed

#downgrade mingw-gcc as ring-daemon is not yet compatible with v5
RUN wget http://seblu.net/a/arm/2015/05/01/community/os/x86_64/mingw-w64-gcc-4.9.2-2-x86_64.pkg.tar.xz
RUN sudo pacman -U mingw-w64-gcc-4.9.2-2-x86_64.pkg.tar.xz --noconfirm

#set env to include perl package (pod2man needed for doc)
ENV PATH=$PATH:/usr/bin/core_perl
RUN env

#install Qt deps
RUN yaourt -S mingw-w64-harfbuzz --noconfirm --needed
RUN yaourt -S mingw-w64-libdbus --noconfirm --needed
RUN yaourt -S mingw-w64-libjpeg-turbo --noconfirm --needed
RUN yaourt -S mingw-w64-libpng --noconfirm --needed
RUN yaourt -S mingw-w64-openssl --noconfirm --needed
RUN yaourt -S mingw-w64-pcre --noconfirm --needed
RUN yaourt -S mingw-w64-sqlite --noconfirm --needed
RUN yaourt -S mingw-w64-zlib --noconfirm --needed
RUN yaourt -S mingw-w64-mariadb-connector-c --noconfirm --needed
RUN yaourt -S mingw-w64-postgresql-libs --noconfirm --needed
RUN yaourt -S mingw-w64-mariadb-connector-c --noconfirm --needed
RUN yaourt -S mingw-w64-postgresql-libs --noconfirm --needed


#install Qt
RUN yaourt -S mingw-w64-qt5-base-opengl --noconfirm

#install the Qt plugins used in the client
RUN yaourt -S mingw-w64-qt5-svg --noconfirm
RUN yaourt -S mingw-w64-qt5-imageformats --noconfirm

#install the packager
RUN yaourt -S nsis --noconfirm --needed

#make sure pkg-config choose our custom path before any lib that may have been installed by Qt (e.g pcre)
RUN sudo sed -i s/\$PKG_CONFIG_LIBDIR:\$PKG_CONFIG_PATH_CUSTOM/\$PKG_CONFIG_PATH_CUSTOM:\$PKG_CONFIG_LIBDIR/ /usr/bin/i686-w64-mingw32-pkg-config