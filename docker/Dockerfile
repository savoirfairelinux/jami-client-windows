
FROM win32client

#add user
RUN useradd joulupukki -m
RUN echo "joulupukki ALL= NOPASSWD: ALL" >> /etc/sudoers

USER joulupukki
WORKDIR /home/joulupukki

#install the packager
RUN yaourt -S nsis --noconfirm --needed
