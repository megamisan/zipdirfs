#!/usr/bin/env bash

set -e
set -u

buildPackage() {
  DIR="$(basename "$(pwd)")"
  tar cf /tmp/archive.tar -C ../ --exclude="${DIR}/${DIR}" --exclude="${DIR}/.git" "${DIR}"
  tar xf /tmp/archive.tar
  (
    cd "${DIR}"
    dpkg-buildpackage -B --no-sign
  )
  test "$(find -maxdepth 1 -name '*.deb' | wc -l)" == "2"
}
apt update -qq
apt install -y --no-install-recommends ca-certificates curl
(. /etc/os-release; echo 'deb https://packages.megami.fr/debian/ '${VERSION_CODENAME}' main' > /etc/apt/sources.list.d/megami.list)
curl -sSL https://packages.megami.fr/pubkey.asc > /etc/apt/trusted.gpg.d/megami.asc
apt update -qq
yes | (cd /tmp; DEBIAN_FRONTEND=noninteractive mk-build-deps --install "${CI_PROJECT_DIR}/debian/control" )

buildPackage

VERSION="$(dpkg-parsechangelog --show-field Version)"
if [ "${VERSION}" != "${CI_COMMIT_TAG}" ] && [ "${CI_PIPELINE_SOURCE}" != "merge_request_event" ]; then
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

workflow:
  rules:
    - if: '\$CI_MERGE_REQUEST_ID'
    - if: '\$CI_COMMIT_TAG'

.common_build:
  stage: build
  script:
    - |
      echo Building package
      bash "\${CI_PROJECT_DIR}/bin/build/build-task.sh"

EOY
if [ "${CI_PIPELINE_SOURCE}" != "merge_request_event" ]; then
  cat <<EOY
.common_deploy:
  stage: deploy
  script:
    - |
      echo Upload to server
      bash "\${CI_PROJECT_DIR}/bin/build/deploy-task.sh" "\${TARGET_DISTRIBUTION}"

EOY
fi

while read -u 3 version distribution suffix; do
  cat <<EOY
build_for_${version}:
  extends: .common_build
  image:
    name: registry.megami.fr:443/management/runner-images/native-cpp:${version}
  artifacts:
    expire_in: 1 day
    paths:
      - zipdirfs*_${VERSION}+${suffix}*
  variables:
    SUFFIX: "${suffix}"

EOY
  if [ "${CI_PIPELINE_SOURCE}" != "merge_request_event" ]; then
    cat <<EOY
deploy_for_${version}:
  extends: .common_deploy
  needs:
    - build_for_${version}
  image:
    name: registry.megami.fr:443/management/runner-images/deploy:latest
  variables:
    TARGET_DISTRIBUTION: "${distribution}"
    SUFFIX: "${suffix}"

EOY
  fi
done 3< "${BUILD_TARGETS}"
