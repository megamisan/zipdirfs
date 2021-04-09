# ZipDirFs

Linux FUSE exposing zip file content under a specified source path.

## Requirements

Following development packages are required:
* libfuse-dev
* libfusekit-dev (https://github.com/multicast/fusekit/)
* libzip-dev
* pkg-config

## Installation

## Debian packages

## Development environment

A minimal environment which can be used with Visual Studio Code is provided in [tools/dev-env](tools/dev-env) using a
docker image as base. It is possible to use the image as is with VSCode or convert it to a WSL2 distro.

To build the image and convert it to a distro:

1. Move to the `dev-env` folder;
1. Build the image: `docker build -t zipdirfs-devenv:latest`;
1. Create a container: `docker create --name zipdirfs-devenv zipdirfs-devenv:latest`;
1. Move to a folder where to export the container;
1. Export the container as archive: `docker export -o zipdirfs.tar.gz zipdirfs-devenv`
1. Delete the container: `docker rm zipdirfs-devenv`;
1. Delete the image: `docker rmi zipdirfs-devenv:latest`;
1. Move to a folder where you want to create the distros into;
1. Import the archive in WSL: `wsl --import zipdirfs ./zipdirfs …/zipdirfs.tar.gz --version 2`;
   > Changing the path to `zipdirfs.tar.gz` as needed.
1. Launch the distro for first install: `wsl -d zipdirfs`
1. Launch first install script and follow instructions: `/root/firstrun.sh`
1. Stop the distro to take new configuration: `wsl --terminate zipdirfs`
1. Open the distro from Visual Studio Code
