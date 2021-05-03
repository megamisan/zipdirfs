#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Zip directory virtual filesystem],[1.0.0],[pierrick.caillon+zipdirfs@megami.fr],[zipdirfs],[https://github.com/megamisan/zipdirfs])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Werror])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/Main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_LANG([C++])

# Options
AC_ARG_WITH([tests],
  [AS_HELP_STRING([--with-tests],
    [activate test suite @<:@default=check@:>@])],
  [],
  [with_tests=check])
AC_ARG_WITH([gtest],
  [AS_HELP_STRING([--with-gtest],
    [location of google test suite include @<:@default=/usr/src/googletest/googletest/include/@:>@])],
  [],
  [with_gtest=/usr/src/googletest/googletest/include/])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
LT_INIT

# Checks for libraries.
AX_BOOST_BASE([1.62],, [AC_MSG_ERROR([This program needs Boost, but it was not found in your system])])
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM

PKG_CHECK_MODULES([FUSE], [fuse >= 2.4.0])
AC_SUBST([FUSE_CFLAGS])
AC_SUBST([FUSE_LIBS])
PKG_CHECK_MODULES([FUSEKIT], [fusekit >= 0.7.1])
AC_SUBST([FUSEKIT_CFLAGS])
PKG_CHECK_MODULES([LIBZIP], [libzip >= 0.10])
AC_SUBST([LIBZIP_CFLAGS])
AC_SUBST([LIBZIP_LIBS])

LIBS="$LIBS $FUSE_LIBS $LIBZIP_LIBS"
CPPFLAGS="$CPPFLAGS $FUSE_CFLAGS $FUSEKIT_CFLAGS $LIBZIP_CFLAGS"

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h])
AS_IF([test "x$with_tests" != "x"],
  [CPPFLAGS="-I googletest/googletest/include/ -I googletest/googlemock/include/ $CPPFLAGS"])
AS_CASE(["$with_tests"],
  [yes], [AC_CHECK_HEADER([gtest/gtest.h], [HAVE_GTEST_H=1])],
  [no], [],
  [AC_CHECK_HEADER([gtest/gtest.h], [HAVE_GTEST_H=1], [HAVE_GTEST_H=0])])
AM_CONDITIONAL([USE_TESTS], [test "$with_tests" != no -a "$HAVE_GTEST_H" -eq 1])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS([clock_gettime realpath])

# Finishing
AM_SANITY_CHECK

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
