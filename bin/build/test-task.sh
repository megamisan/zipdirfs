#!/usr/bin/env bash

set -e
set -u

PROJECT_FOLDER=zipdirfs
apt update -qq
apt install -y --no-install-recommends ca-certificates curl locales-all
echo 'deb https://packages.megami.fr/debian/ buster main' > /etc/apt/sources.list.d/megami.list
curl -sSL https://packages.megami.fr/pubkey.asc > /etc/apt/trusted.gpg.d/megami.asc
apt update -qq
yes | (cd /tmp; DEBIAN_FRONTEND=noninteractive mk-build-deps --install "${CI_PROJECT_DIR}/debian/control" )
apt install -y --no-install-recommends googletest google-mock
cp -ra /usr/src/googletest "${CI_PROJECT_DIR}/"
(
  cd "${CI_PROJECT_DIR}/"
  autoreconf --install
  ./configure --with-tests
  make
  make gtest
  ./gtest --gtest_filter=-ZipDirectoryEnumeratorTest.AutoCleanup
)
