ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I $(top_srcdir)/include
AM_CXXFLAGS = -Wall -fexceptions
AM_LDFLAGS = -Ur

bin_PROGRAMS = zipdirfs
zipdirfs_SOURCES = \
	src/Main.cpp \
	src/Options.cpp
zipdirfs_CXXFLAGS = $(CXXFLAGS_COVERAGE)
zipdirfs_LDADD = $(LDADD_COVERAGE) libzipdirfs.la

noinst_LTLIBRARIES = libzipdirfs.la libzipdirfs-test.la
libzipdirfs_la_SOURCES = \
	src/ZipDirFs/EntryFactory.cpp \
	src/ZipDirFs/MutexLockPolicy.cpp \
	src/ZipDirFs/NotFoundException.cpp \
	src/ZipDirFs/stream_indent.cpp \
	src/ZipDirFs/SymlinkTime.cpp \
	src/ZipDirFs/SystemDirectoryFactory.cpp \
	src/ZipDirFs/ZipEntry.cpp \
	src/ZipDirFs/ZipEntryFileInfo.cpp \
	src/ZipDirFs/ZipFile.cpp \
	src/ZipDirFs/ZipIterator.cpp \
	src/ZipDirFs/ZipWalker.cpp
libzipdirfs_la_CXXFLAGS = $(CXXFLAGS_COVERAGE)
libzipdirfs_la_LIBADD = $(LDADD_COVERAGE)

noinst_HEADERS = \
	include/Main.h \
	include/Options.h \
	include/ZipDirFs/DefaultPermission.h \
	include/ZipDirFs/DirectoryMark.h \
	include/ZipDirFs/DirectoryNode.h \
	include/ZipDirFs/entry_definitions.h \
	include/ZipDirFs/EntryFactory.h \
	include/ZipDirFs/FixedDirectoryFactory.h \
	include/ZipDirFs/MutexLockPolicy.h \
	include/ZipDirFs/NameSearchTree.h \
	include/ZipDirFs/NameSearchTreeNodes.h \
	include/ZipDirFs/NameSearchWalker.h \
	include/ZipDirFs/NotFoundException.h \
	include/ZipDirFs/StatDirectoryFactory.h \
	include/ZipDirFs/stream_indent.h \
	include/ZipDirFs/SymlinkTime.h \
	include/ZipDirFs/SystemDirectoryFactory.h \
	include/ZipDirFs/SystemDirectoryTime.h \
	include/ZipDirFs/UnionDirectoryFactory.h \
	include/ZipDirFs/utils.h \
	include/ZipDirFs/ZipDirectoryFactory.h \
	include/ZipDirFs/ZipEntry.h \
	include/ZipDirFs/ZipEntryFileInfo.h \
	include/ZipDirFs/ZipFile.h \
	include/ZipDirFs/ZipFileBuffer.h \
	include/ZipDirFs/ZipIterator.h \
	include/ZipDirFs/ZipRootFactory.h \
	include/ZipDirFs/ZipTime.h \
	include/ZipDirFs/ZipWalker.h

libzipdirfs_test_la_SOURCES = test/old/tests.cpp
libzipdirfs_test_la_LIBADD = libzipdirfs.la

noinst_PROGRAMS = zipdirfs-test1 zipdirfs-test2 zipdirfs-test3 zipdirfs-test4 \
	 zipdirfs-test5 zipdirfs-test6 zipdirfs-test7 zipdirfs-test8
zipdirfs_test1_SOURCES = test/old/maintest.cpp
zipdirfs_test1_CPPFLAGS = -D TEST=1
zipdirfs_test1_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test2_SOURCES = test/old/maintest.cpp
zipdirfs_test2_CPPFLAGS = -D TEST=2
zipdirfs_test2_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test3_SOURCES = test/old/maintest.cpp
zipdirfs_test3_CPPFLAGS = -D TEST=3
zipdirfs_test3_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test4_SOURCES = test/old/maintest.cpp
zipdirfs_test4_CPPFLAGS = -D TEST=4
zipdirfs_test4_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test5_SOURCES = test/old/maintest.cpp
zipdirfs_test5_CPPFLAGS = -D TEST=5
zipdirfs_test5_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test6_SOURCES = test/old/maintest.cpp
zipdirfs_test6_CPPFLAGS = -D TEST=6
zipdirfs_test6_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test7_SOURCES = test/old/maintest.cpp
zipdirfs_test7_CPPFLAGS = -D TEST=7
zipdirfs_test7_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)
zipdirfs_test8_SOURCES = test/old/maintest.cpp
zipdirfs_test8_CPPFLAGS = -D TEST=8
zipdirfs_test8_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE)

if USE_TESTS
check_LTLIBRARIES = libgtest.la libgmock_main.la libgtest_local.la
libgtest_la_SOURCES = \
	googletest/googletest/src/gtest-all.cc
libgtest_la_CPPFLAGS = -I googletest/googletest/include -I googletest/googletest
libgtest_la_LDFLAGS = -pthread
libgmock_main_la_SOURCES = \
	googletest/googlemock/src/gmock-all.cc \
    googletest/googlemock/src/gmock_main.cc
libgmock_main_la_CPPFLAGS = -I googletest/googlemock/include -I googletest/googlemock
libgmock_main_la_LDFLAGS = -pthread
libgmock_main_la_LIBADD = libgtest.la
libgtest_local_la_SOURCES = \
	test/Coverage/coverage.cpp
libgtest_local_la_CXXFLAGS = $(CXXFLAGS_COVERAGE) -D BUILD_TEST
libgtest_local_la_LIBADD = $(LDADD_COVERAGE)
libgtest_local_la_LDFLAGS = -pthread

check_PROGRAMS = gtest
gtest_SOURCES = \
	test/gtest.h \
	test/gtest.cpp
gtest_CXXFLAGS = $(CXXFLAGS_COVERAGE)
gtest_LDADD = libzipdirfs.la libgtest.la libgmock_main.la libgtest_local.la $(LDADD_COVERAGE)
gtest_LDFLAGS = -pthread

gtest-coverage: gtest
	bin/build-coverage.sh gtest "$(abs_top_builddir)"

gtest-coverage-case: gtest
	bin/build-case-coverage.sh gtest "$(abs_top_builddir)"

gtest-coverage-all: gtest
	bin/build-detailed-coverage.sh gtest "$(abs_top_builddir)"

clean-coverage:
	find coverage/ -name '*.info' -print0 | xargs -0r rm

TESTS = gtest
endif
