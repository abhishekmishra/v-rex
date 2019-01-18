FROM ubuntu:latest
LABEL maintainer="Abhishek Mishra <abhishekmishra3@gmail.com>"

# get ubuntu updated.
RUN apt-get update -y
RUN apt-get upgrade -y

# install project dependencies.
RUN apt-get install -y gcc make libxbae-dev libxmhtml-dev libxt-dev libmotif-dev libjson-c-dev libcurl4-openssl-dev libxmu-dev

# install wget to download the latest tarballs.
RUN apt-get install -y wget

# download and build movingmotif
RUN apt-get install -y libxpm-dev
WORKDIR /root/movingmotif
RUN wget https://sourceforge.net/projects/movingmotif/files/movingmotif_2_1/movingmotif-2.2.0/movingmotif-2.2.0.tar.gz
RUN tar --strip=1 -xzvf movingmotif-2.2.0.tar.gz
RUN ./configure --prefix=/root/movingmotif
RUN make
RUN make install
RUN ./libtool --finish /root/movingmotif
ENV LD_LIBRARY_PATH="/root/movingmotif/lib:${LD_LIBRARY_PATH}"