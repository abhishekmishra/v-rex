FROM abhishekmishra3/v-rex-base:latest
LABEL maintainer="Abhishek Mishra <abhishekmishra3@gmail.com>"

# download and build clibdocker.
WORKDIR /root/clibdocker
RUN wget https://github.com/abhishekmishra/clibdocker/tarball/master -O clibdocker.tar.gz
RUN tar --strip=1 -xzvf clibdocker.tar.gz
RUN make

# download and build v-rex-motif
WORKDIR /root/v-rex
RUN wget https://github.com/abhishekmishra/v-rex/tarball/master -O v-rex.tar.gz
RUN tar --strip=1 -xzvf v-rex.tar.gz
WORKDIR /root/v-rex/v-rex-motif
RUN make
