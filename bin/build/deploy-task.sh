#!/usr/bin/env bash

set -e
set -u

DISTRIBUTION="$1"

cp "${CI_PROJECT_DIR}/zipdirfs"*"_${VERSION}+${SUFFIX}"* "${CI_PROJECT_DIR}/bin/build/deploy-task-exec.sh" /tmp/
chmod a+x /tmp/deploy-task-exec.sh

chmod go= "${APTLY_SERVER_SSH_KEY}"
(cd /tmp; find . -\( -name "zipdirfs*_${VERSION}+${SUFFIX}*" -or -name deploy-task-exec.sh -\) -print0) | cut -z -c3- | tar cz -C /tmp/ --null -T - | \
ssh -i "${APTLY_SERVER_SSH_KEY}" -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no "${APTLY_SERVER_SSH}" -- trap "'rm -f \"/tmp/zipdirfs\"*\"_${VERSION}+${SUFFIX}*\" /tmp/deploy-task-exec.sh'" EXIT \;\
  tar xz -C /tmp/\; /tmp/deploy-task-exec.sh "'${DISTRIBUTION}'" "'${APTLY_PUBLISH_ENDPOINT}'" "'${APTLY_KEY_PASSWORD}'"
