FROM ubuntu:xenial
MAINTAINER 2018s-gitctf-team1

USER root

RUN DIST=xenial && \
    sed -i 's/deb.debian.org/ftp.daumkakao.com/' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends --no-install-suggests \
    make git libgpgme11-dev libncurses-dev g++ libcurl4-openssl-dev \
    alsa-utils
RUN apt-get install -y apt-utils
RUN apt-get install -y ca-certificates
RUN apt-get install -y python-pip python-dev build-essential && \
    pip install --upgrade pip
RUN pip install numpy && \
    pip install bitarray

RUN rm -rf /var/lib/apt/lists/* && \
    apt-get clean


WORKDIR /root
CMD ["/bin/bash"]

