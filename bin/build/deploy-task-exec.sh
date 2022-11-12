#!/usr/bin/env bash

set -e
set -u

distribution="$1"
endpoint="$2"
password="$3"

aptly repo include -accept-unsigned -force-replace -ignore-signatures /tmp
aptly publish update -batch -passphrase="${password}" "${distribution}" "${endpoint}"
