#!/usr/bin/env bash

set -e
set -u

buildPackage() {
  DIR="$(basename "$(pwd)")"
  tar c -C ../ --exclude="${DIR}/${DIR}" --exclude="${DIR}/.git" "${DIR}" | tar x
  (
    cd "${DIR}"
    dpkg-buildpackage -G --no-sign
  )
  test "$(find -maxdepth 1 -\( -name '*.dsc' -or -name '*.deb' -\) | wc -l)" == "3"
}

(. /etc/os-release; sed -i "s/unstable/${VERSION_CODENAME}/" "${CI_PROJECT_DIR}/debian/control")
sed -i "s/(${VERSION})/(${VERSION}+${SUFFIX})/" "${CI_PROJECT_DIR}/debian/changelog"
apt update -qq
apt install -y --no-install-recommends ca-certificates curl
echo 'deb https://packages.megami.fr/debian/ buster main' > /etc/apt/sources.list.d/megami.list
curl -sSL https://packages.megami.fr/pubkey.asc > /etc/apt/trusted.gpg.d/megami.asc
apt update -qq
yes | (cd /tmp; DEBIAN_FRONTEND=noninteractive mk-build-deps --install "${CI_PROJECT_DIR}/debian/control")

buildPackage
