# ZipDirFs

Linux FUSE exposing zip file content under a specified source path.

When mounting a file system with this program, you will find in your mount point:

* Same permissions and owners as in the source (TODO);
* Access too all existing extended attributes (TODO);
* System folders shown as-is;
* Zip file as directory, keeping the file extension;
* Zip content in the zip file directory;
* Other files and nodes as symbolink link to their original location;

The mounted file system is always read-only.

## Requirements

Following development packages are required:

* libfuse-dev
* libfusekit-dev (<https://github.com/multicast/fusekit/>)
* libzip-dev
* pkg-config

## Installation

The package use autotools for building.

Standard procedure from distributed package:

```shell script
./configure
make
sudo make install
```

When compiling from source, run `autoreconf --install` first.

## Debian packages

Package building has been tested under debian buster.

To build the package, run `dpkg-buildpackage -B`.

## Testing

Test are written using [Google Test Framework](https://github.com/google/googletest).

Test are to be run with version 1.10.0 or later of the framework.
Tentative Debian or Ubuntu packages:

* googletest
* google-mock

To active tests in build, copy `/usr/src/googletest` to project dir and add `--with-tests` to `./configure`.

On Debian Buster, use backports.

## Development environment

A minimal environment which can be used with Visual Studio Code is provided in [tools/dev-env](tools/dev-env) using a
docker image as base. It is possible to use the image as is with VSCode or convert it to a WSL2 distro.

To build the image and convert it to a distro:

1. Move to the `dev-env` folder;
1. Build the image: `docker build -t zipdirfs-devenv:`*version*` Dockerfile.`*version*;
1. Create a container: `docker create --name zipdirfs-devenv zipdirfs-devenv:`*version*;
1. Move to a folder where to export the container;
1. Export the container as archive: `docker export -o zipdirfs.tar.gz zipdirfs-devenv`
1. Delete the container: `docker rm zipdirfs-devenv`;
1. Delete the image: `docker rmi zipdirfs-devenv:`*version*;
1. Move to a folder where you want to create the distros into;
1. Import the archive in WSL: `wsl --import zipdirfs ./zipdirfs …/zipdirfs.tar.gz --version 2`;
   > Changing the path to `zipdirfs.tar.gz` as needed.
1. Launch the distro for first install: `wsl -d zipdirfs`
1. Launch first install script and follow instructions: `/root/firstrun.sh`
1. Stop the distro to take new configuration: `wsl --terminate zipdirfs`
1. Open the distro from Visual Studio Code

Où *version* est la version de la distribution souhaitée. Voir [le contenu de dev-env](tools/dev-env).

## Coverage

For coverage visualisation, these make variables are needed:

* `CXXFLAGS_COVERAGE="--coverage -fno-inline -fno-inline-small-functions -fno-default-inline -fno-elide-constructors"`
* `LDADD_COVERAGE="-lgcov"`
* `CFLAGS="-g -O0"`
* `CXXFLAGS="-g -O0"`

To build unit tests with required flags:

```shell
make CXXFLAGS_COVERAGE="--coverage -fno-inline -fno-inline-small-functions -fno-default-inline -fno-elide-constructors" LDADD_COVERAGE="-lgcov" CFLAGS="-g -O0" CXXFLAGS="-g -O0" gtest
```

Also, this package is needed to get HTML visualisations:

* lcov

The commands, after running test binary `gtest` are:

```shell
lcov --no-external --capture --initial --directory . \
  --output-file coverage/zipdirfs_base.info --config-file=lcovrc ; \
lcov --no-external --capture --directory . \
  --output-file coverage/zipdirfs_gtest.info --config-file=lcovrc ; \
lcov --add-tracefile coverage/zipdirfs_base.info \
  --add-tracefile coverage/zipdirfs_gtest.info \
  --output-file coverage/zipdirfs_gtest_total.info --config-file=lcovrc ; \
lcov --remove coverage/zipdirfs_gtest_total.info \
  '/usr/include/*' '/usr/lib/*' "$(pwd)"'/test/*' "$(pwd)"'/googletest/*' \
  -o coverage/zipdirfs_gtest_filtered.info --config-file=lcovrc ; \
rm -rf coverage/output/ ; \
mkdir coverage/output/ ; \
genhtml --config-file=lcovrc --prefix $(pwd) --ignore-errors source \
  coverage/zipdirfs_gtest_filtered.info \
--legend --title "gtest coverage" --output-directory=coverage/output/
```

A make target has been created to facilitate generating the result: `gtest-coverage`.
