#!/usr/bin/env bash

set -e
set -u

count="$1"
shift
packages=()

while [ "${count}" -gt 0 ]; do
  packages+=("$1")
  shift
  count=$((count - 1))
done

repository="$1"
distribution="$2"
endpoint="$3"
password="$4"

aptly repo add -force-replace "${repository}" "${packages[@]}"
aptly publish update -batch -passphrase="${password}" "${distribution}" "${endpoint}"
