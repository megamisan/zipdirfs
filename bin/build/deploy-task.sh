#!/usr/bin/env bash

set -e
set -u

REPOSITORY="$1"
DISTRIBUTION="$2"

cp "${CI_PROJECT_DIR}/zipdirfs_${VERSION}+${SUFFIX}_amd64.deb" "${CI_PROJECT_DIR}/zipdirfs-dbgsym_${VERSION}+${SUFFIX}_amd64.deb" "${CI_PROJECT_DIR}/zipdirfs_${VERSION}+${SUFFIX}.dsc" "${CI_PROJECT_DIR}/zipdirfs_${VERSION}+${SUFFIX}.tar.xz" "${CI_PROJECT_DIR}/bin/build/deploy-task-exec.sh" /tmp/
chmod a+x /tmp/deploy-task-exec.sh

tar cz -C /tmp/ "zipdirfs_${VERSION}+${SUFFIX}_amd64.deb" "zipdirfs-dbgsym_${VERSION}+${SUFFIX}_amd64.deb" "zipdirfs_${VERSION}+${SUFFIX}.dsc" "zipdirfs_${VERSION}+${SUFFIX}.tar.xz" deploy-task-exec.sh | \
ssh "${APTLY_SERVER_SSH}" -- trap "'rm -f \"/tmp/zipdirfs_${VERSION}+${SUFFIX}_amd64.deb\" \"/tmp/zipdirfs-dbgsym_${VERSION}+${SUFFIX}_amd64.deb\" \"/tmp/zipdirfs_${VERSION}+${SUFFIX}.dsc\" \"/tmp/zipdirfs_${VERSION}+${SUFFIX}.tar.xz\" /tmp/deploy-task-exec.sh'" EXIT \;\
  tar xz -C /tmp/\; /tmp/deploy-task-exec.sh 3 "'/tmp/zipdirfs_${VERSION}+${SUFFIX}_amd64.deb'" "'/tmp/zipdirfs-dbgsym_${VERSION}+${SUFFIX}_amd64.deb'" "'/tmp/zipdirfs_${VERSION}+${SUFFIX}.dsc'" "'${REPOSITORY}'" "'${DISTRIBUTION}'" "'${APTLY_PUBLISH_ENDPOINT}'" "'${APTLY_KEY_PASSWORD}'"
