#!/usr/bin/env bash

set -e
set -u

DISTRIBUTION="$1"

: "${VERSION}" "${SUFFIX}" "${APTLY_SSH_PRIVATE_KEY}" "${APTLY_SSH_SERVER}" "${SSH_HOST_KEY}" "${APTLY_PUBLISH_ENDPOINT}" "${APTLY_KEY_PASSWORD}"

cp "${CI_PROJECT_DIR}/zipdirfs"*"_${VERSION}+${SUFFIX}"* "${CI_PROJECT_DIR}/bin/build/deploy-task-exec.sh" /tmp/
chmod a+x /tmp/deploy-task-exec.sh

chmod go= "${APTLY_SSH_PRIVATE_KEY}" "${SSH_HOST_KEY}"
(umask 077; if [ ! -d ~/.ssh ]; then mkdir ~/.ssh; fi; cat > ~/.ssh/config) <<EOC
IdentityFile ${APTLY_SSH_PRIVATE_KEY}
UserKnownHostsFile ${SSH_HOST_KEY}

EOC
(cd /tmp; find . -\( -name "zipdirfs*_${VERSION}+${SUFFIX}*" -or -name deploy-task-exec.sh -\) -print0) | cut -z -c3- | tar cz -C /tmp/ --null -T - | \
ssh "${APTLY_SSH_SERVER}" -- trap "'rm -f \"/tmp/zipdirfs\"*\"_${VERSION}+${SUFFIX}*\" /tmp/deploy-task-exec.sh'" EXIT \;\
  tar xz -C /tmp/\; /tmp/deploy-task-exec.sh "'${DISTRIBUTION}'" "'${APTLY_PUBLISH_ENDPOINT}'" "'${APTLY_KEY_PASSWORD}'"
