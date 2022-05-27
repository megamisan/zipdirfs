#!/usr/bin/env bash

set -e
set -u

buildPackage() {
  DIR="$(basename "$(pwd)")"
  tar c -C ../ --exclude="${DIR}/${DIR}" --exclude="${DIR}/.git" "${DIR}" | tar x
  (
    cd "${DIR}"
    dpkg-buildpackage -B --no-sign
  )
  test "$(find -maxdepth 1 -name '*.deb' | wc -l)" == "2"
}
apt update -qq
apt install -y --no-install-recommends ca-certificates curl
echo 'deb https://packages.megami.fr/debian/ buster main' > /etc/apt/sources.list.d/megami.list
curl -sSL https://packages.megami.fr/pubkey.asc > /etc/apt/trusted.gpg.d/megami.asc
apt update -qq
yes | (cd /tmp; DEBIAN_FRONTEND=noninteractive mk-build-deps --install "${CI_PROJECT_DIR}/debian/control" )

buildPackage

VERSION="$(dpkg-parsechangelog --show-field Version)"
if [ "${VERSION}" != "${CI_COMMIT_TAG}" ]; then
  cat > "generated-commands.yml" <<EOY
nothing:
  stage: test
  script:
    - "true"

EOY
  exit
fi

exec > "generated-commands.yml"
cat <<EOY
variables:
  VERSION: ${VERSION}

.common_build:
  stage: build
  artifacts:
    expire_in: 1 day
    paths:
      - zipdirfs_${VERSION}+\${SUFFIX}_amd64.deb
      - zipdirfs-dbgsym_${VERSION}+\${SUFFIX}_amd64.deb
      - zipdirfs_${VERSION}+\${SUFFIX}.dsc
      - zipdirfs_${VERSION}+\${SUFFIX}.tar.xz
  script:
    - |
      echo Building package
      bash "\${CI_PROJECT_DIR}/bin/build/build-task.sh"

.common_deploy:
  stage: deploy
  script:
    - |
      echo Upload to server
      bash "\${CI_PROJECT_DIR}/bin/build/deploy-task.sh" "\${TARGET_REPOSITORY}" "\${TARGET_DISTRIBUTION}"

EOY

while read -u 3 version repository distribution suffix; do
  cat <<EOY
build_for_${version}:
  extends: .common_build
  image:
    name: registry.megami.fr:443/management/runner-images/native-cpp:${version}
  variables:
    SUFFIX: "${suffix}"

deploy_for_${version}:
  extends: .common_deploy
  needs:
    - build_for_${version}
  image:
    name: registry.megami.fr:443/management/runner-images/deploy:latest
  variables:
    TARGET_REPOSITORY: "${repository}"
    TARGET_DISTRIBUTION: "${distribution}"
    SUFFIX: "${suffix}"

EOY
done 3< "${BUILD_TARGETS}"
