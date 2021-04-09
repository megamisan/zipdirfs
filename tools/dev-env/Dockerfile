FROM debian:buster-slim

RUN set -eux; \
    apt-get -qq update; \
    apt-get -qq upgrade -y; \
    apt-get -qq install -y --no-install-recommends \
    build-essential libfuse-dev libzip-dev pkg-config \
    debhelper autotools-dev fuse openssh-client \
    git sudo ca-certificates lcov googletest google-mock \
    bash-completion nano vim locales-all \
    procps iotop lsof; \
    rm -rf /var/lib/apt/lists/*;

RUN set -eux; \
    cd /opt; \
    git clone https://github.com/megamisan/fusekit.git; \
    cd fusekit; \
    dpkg-buildpackage; \
    cd ..; \
    dpkg -i *.deb; \
    rm -rf /opt/fusekit /opt/libfusekit*

COPY firstrun.sh /root/firstrun.sh
RUN chmod a+x /root/firstrun.sh
