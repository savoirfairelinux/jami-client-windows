
FROM win32client

#add user
RUN useradd joulupukki -m
RUN echo "joulupukki ALL= NOPASSWD: ALL" >> /etc/sudoers

USER joulupukki
WORKDIR /home/joulupukki

#set env to include perl package (pod2man needed for doc)
ENV PATH=$PATH:/usr/bin/core_perl
RUN env

#install the packager
RUN yaourt -Syu --noconfirm --needed
RUN yaourt -S nsis --noconfirm --needed
