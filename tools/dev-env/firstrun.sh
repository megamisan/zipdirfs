#!/usr/bin/env bash

set -e
set -u

isValidUsername() {
  local re='^[[:lower:]_][[:lower:][:digit:]_-]{2,15}$'
  (( ${#1} > 16 )) && return 1
  [ "${1}" == "root" ] && return 1
  [[ $1 =~ $re ]] # return value of this comparison is used for the function
}

echo First start. We need to create the default user.
while [ -z "${username:-}" ] || ! isValidUsername "${username:-}"; do
    echo -n 'New user name: '
    read -r username </dev/tty
    if ! isValidUsername "${username}"; then
      echo Invalid user name entered. Must be lower case and between 2 and 15 characters.
    fi
done
while ! adduser --quiet --gecos '' "${username}"; do
  true
done
usermod -aG adm,cdrom,sudo,dip,plugdev "${username}"
cat >/etc/wsl.conf <<EOC
[user]
default = ${username}
EOC
echo User created. Use \"sudo\" to run as root. See \"man sudo_root\".
echo Close terminal and stop distro
rm /etc/resolv.conf
rm "$0"
