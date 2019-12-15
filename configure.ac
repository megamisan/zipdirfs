#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Zip directory virtual filesystem],[0.2.0],[pierrick.caillon+zipdirfs@megami.fr],[zipdirfs],[https://github.com/megamisan/zipdirfs])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_SRCDIR([src/Main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR

# Checks for libraries.
PKG_CHECK_MODULES([FUSE], [fuse >= 2.4.0])
AC_SUBST([FUSE_CFLAGS])
AC_SUBST([FUSE_LIBS])

LIBS="$LIBS $FUSE_LIBS"
CPPFLAGS="$CPPFLAGS $FUSE_CFLAGS"

PKG_CHECK_MODULES([FUSEKIT], [fusekit >= 0.6.3])
AC_SUBST([FUSEKIT_CFLAGS])

CPPFLAGS="$CPPFLAGS $FUSEKIT_CFLAGS"

PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.10])
AC_SUBST([LIBZIP_CFLAGS])
AC_SUBST([LIBZIP_LIBS])

LIBS="$LIBS $LIBZIP_LIBS"
CPPFLAGS="$CPPFLAGS $LIBZIP_CFLAGS"

AM_SANITY_CHECK

AC_LANG([C++])

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_DIRENT
AC_HEADER_STAT
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h string.h unistd.h stddef.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([pathconf])
AC_CHECK_FUNCS([realpath])

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile src/ZipDirFs/Makefile])

AC_OUTPUT
