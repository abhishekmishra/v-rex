FROM ubuntu:latest
LABEL maintainer="Abhishek Mishra <abhishekmishra3@gmail.com>"

RUN apt-get update -y
RUN apt-get upgrade -y
RUN apt-get install -y gcc make libxbae-dev libxmhtml-dev libxt-dev libmotif-dev libjson-c-dev libcurl4-openssl-dev
RUN apt-get install -y libxmu-dev
RUN apt-get install -y wget

WORKDIR /root/clibdocker
RUN wget https://github.com/abhishekmishra/clibdocker/tarball/master -O clibdocker.tar.gz
RUN tar --strip=1 -xzvf clibdocker.tar.gz
RUN make

WORKDIR /root/v-rex
RUN wget https://github.com/abhishekmishra/v-rex/tarball/master -O v-rex.tar.gz
RUN tar --strip=1 -xzvf v-rex.tar.gz
WORKDIR /root/v-rex/v-rex-motif
RUN make
