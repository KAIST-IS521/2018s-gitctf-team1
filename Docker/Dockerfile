FROM ubuntu:xenial
MAINTAINER 2018s-gitctf-team1

ENV DEBIAN_FRONTEND noninteractive

USER root

RUN DIST=xenial && \
    sed -i 's/deb.debian.org/ftp.daumkakao.com/' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends --no-install-suggests \
    make git libgpgme11-dev libncurses-dev g++ libcurl4-openssl-dev \
    alsa-utils
RUN apt-get install -y apt-utils
RUN apt-get install -y ca-certificates
RUN echo "mysql-server mysql-server/root_password password root" | debconf-set-selections
RUN echo "mysql-server mysql-server/root_password_again password root" | debconf-set-selections
RUN apt-get install -y mysql-server
WORKDIR /etc/mysql
RUN sed -i "s/127.0.0.1/0.0.0.0/g" my.cnf

RUN apt-get install -y python-pip python-dev build-essential && \
    pip install --upgrade pip
RUN pip install numpy && \
    pip install bitarray && \
    pip install six && \
    pip install PyMySQL
RUN rm -rf /var/lib/apt/lists/* && \
    apt-get clean

ADD test.sh /root/test.sh
RUN chmod +x /root/test.sh

WORKDIR /root
CMD ["/root/test.sh"]

