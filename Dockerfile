###############################################################################
# Git-based CTF
###############################################################################
#
# Author: SeongIl Wi <seongil.wi@kaist.ac.kr>
#         Jaeseung Choi <jschoi17@kaist.ac.kr>
#         Sang Kil Cha <sangkilc@kaist.ac.kr>
#
# Copyright (c) 2018 SoftSec Lab. KAIST
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

FROM debian:latest

# ======================================
# Install your package here
MAINTAINER 2018s-gitctf-team1
ENV DEBIAN_FRONTEND noninteractive
RUN DIST=xenial && \
    sed -i 's/deb.debian.org/ftp.daumkakao.com/' /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y python-pip python-dev build-essential mysql-server make \
                       gcc g++ libcurl4-openssl-dev

#RUN echo "mysql-server mysql-server/root_password password root" | debconf-set-selections
#RUN echo "mysql-server mysql-server/root_password_again password root" | debconf-set-selections
#RUN apt-get install -y mysql-server
WORKDIR /etc/mysql

RUN pip install --upgrade pip numpy bitarray pyMySQL
RUN rm -rf /var/lib/apt/lists/* && \
    apt-get clean

# ======================================

RUN mkdir -p /var/ctf
COPY flag /var/ctf/

# ======================================
# Build and run your service here
ADD hackttp /hackttp
ADD Service /var/www/cgi

ADD init.sh /init.sh
ADD init_db.sh /init_db.sh
RUN chmod +x /init.sh
RUN chmod +x /init_db.sh

RUN /init_db.sh

RUN make -C /hackttp

CMD ["/init.sh"]
# ======================================
