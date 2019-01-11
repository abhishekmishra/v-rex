FROM ubuntu:latest
LABEL maintainer="Abhishek Mishra <abhishekmishra3@gmail.com>"

# get ubuntu updated.
RUN apt-get update -y
RUN apt-get upgrade -y

# install project dependencies.
RUN apt-get install -y gcc make libxbae-dev libxmhtml-dev libxt-dev libmotif-dev libjson-c-dev libcurl4-openssl-dev libxmu-dev

# install wget to download the latest tarballs.
RUN apt-get install -y wget