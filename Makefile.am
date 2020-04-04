ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I $(top_srcdir)/include $(BOOST_CPPFLAGS)
AM_CXXFLAGS = -Wall -fexceptions
AM_LDFLAGS = -Ur $(BOOST_LDFLAGS)

bin_PROGRAMS = zipdirfs
zipdirfs_SOURCES = \
	src/Main.cpp \
	src/Options.cpp
zipdirfs_CXXFLAGS = $(CXXFLAGS_COVERAGE)
zipdirfs_LDADD = $(LDADD_COVERAGE) libzipdirfs.la $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)

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
	src/ZipDirFs/ZipWalker.cpp \
	src/ZipDirFs/Components/NativeChanged.cpp \
	src/ZipDirFs/Components/NativeDirectoryEnumerator.cpp \
	src/ZipDirFs/Components/ZipDirectoryEnumerator.cpp \
	src/ZipDirFs/Components/ZipFileChanged.cpp \
	src/ZipDirFs/Containers/EntryGenerator.cpp \
	src/ZipDirFs/Containers/EntryIterator.cpp \
	src/ZipDirFs/Utilities/FileSystem.cpp \
	src/ZipDirFs/Zip/Archive.cpp \
	src/ZipDirFs/Zip/Entry.cpp \
	src/ZipDirFs/Zip/Exception.cpp \
	src/ZipDirFs/Zip/Factory.cpp \
	src/ZipDirFs/Zip/Lib.cpp \
	src/ZipDirFs/Zip/Stream.cpp \
	src/ZipDirFs/Zip/Base/Content.cpp \
	src/ZipDirFs/Zip/Base/Lib.cpp \
	src/ZipDirFs/Zip/Base/Stat.cpp
libzipdirfs_la_CXXFLAGS = $(CXXFLAGS_COVERAGE)
libzipdirfs_la_LIBADD = $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)

noinst_HEADERS = \
	include/Main.h \
	include/Options.h \
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
	include/ZipDirFs/ZipWalker.h \
	include/ZipDirFs/Components/NativeChanged.h \
	include/ZipDirFs/Components/NativeDirectoryEnumerator.h \
	include/ZipDirFs/Components/NativeFactory.h \
	include/ZipDirFs/Components/ZipDirectoryEnumerator.h \
	include/ZipDirFs/Components/ZipFactory.h \
	include/ZipDirFs/Components/ZipFileChanged.h \
	include/ZipDirFs/Containers/EntryGenerator.h \
	include/ZipDirFs/Containers/EntryIterator.h \
	include/ZipDirFs/Containers/EntryList.h \
	include/ZipDirFs/Containers/list_map.h \
	include/ZipDirFs/Containers/Helpers/Changed.h \
	include/ZipDirFs/Containers/Helpers/EntryListProxy.h \
	include/ZipDirFs/Containers/Helpers/Enumerator.h \
	include/ZipDirFs/Containers/Helpers/Factory.h \
	include/ZipDirFs/Utilities/DefaultPermission.h \
	include/ZipDirFs/Utilities/FileSystem.h \
	include/ZipDirFs/Utilities/type_traits.h \
	include/ZipDirFs/Zip/Archive.h \
	include/ZipDirFs/Zip/Entry.h \
	include/ZipDirFs/Zip/Exception.h \
	include/ZipDirFs/Zip/Factory.h \
	include/ZipDirFs/Zip/Lib.h \
	include/ZipDirFs/Zip/Stream.h \
	include/ZipDirFs/Zip/Base/Content.h \
	include/ZipDirFs/Zip/Base/Lib.h \
	include/ZipDirFs/Zip/Base/Stat.h

libzipdirfs_test_la_SOURCES = test/old/tests.cpp
libzipdirfs_test_la_LIBADD = libzipdirfs.la $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)

noinst_PROGRAMS = zipdirfs-test1 zipdirfs-test2 zipdirfs-test3 zipdirfs-test4 \
	 zipdirfs-test5 zipdirfs-test6 zipdirfs-test7 zipdirfs-test8
zipdirfs_test1_SOURCES = test/old/maintest.cpp
zipdirfs_test1_CPPFLAGS = -D TEST=1
zipdirfs_test1_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test2_SOURCES = test/old/maintest.cpp
zipdirfs_test2_CPPFLAGS = -D TEST=2
zipdirfs_test2_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test3_SOURCES = test/old/maintest.cpp
zipdirfs_test3_CPPFLAGS = -D TEST=3
zipdirfs_test3_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test4_SOURCES = test/old/maintest.cpp
zipdirfs_test4_CPPFLAGS = -D TEST=4
zipdirfs_test4_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test5_SOURCES = test/old/maintest.cpp
zipdirfs_test5_CPPFLAGS = -D TEST=5
zipdirfs_test5_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test6_SOURCES = test/old/maintest.cpp
zipdirfs_test6_CPPFLAGS = -D TEST=6
zipdirfs_test6_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test7_SOURCES = test/old/maintest.cpp
zipdirfs_test7_CPPFLAGS = -D TEST=7
zipdirfs_test7_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
zipdirfs_test8_SOURCES = test/old/maintest.cpp
zipdirfs_test8_CPPFLAGS = -D TEST=8
zipdirfs_test8_LDADD = libzipdirfs.la libzipdirfs-test.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)

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
	test/Coverage/coverage.cpp \
	test/fusekit/Fixtures/EntryMock.h \
	test/ZipDirFs/Components/Fixtures/LibInstance.h \
	test/ZipDirFs/Components/Fixtures/ZipFactoryAccess.h \
	test/ZipDirFs/Components/Fixtures/ZipFactoryAccess.cpp \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorEndWrapper.h \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorEndWrapper.cpp \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorTestWrapper.h \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorTestWrapper.cpp \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorWrapperMock.h \
	test/ZipDirFs/Containers/Fixtures/EntryIteratorWrapperMock.cpp \
	test/ZipDirFs/Containers/Fixtures/helpers.h \
	test/ZipDirFs/Containers/Fixtures/helpers.cpp \
	test/ZipDirFs/Containers/Fixtures/HolderStateInitializer.h \
	test/ZipDirFs/Containers/Fixtures/HolderStateInitializer.cpp \
	test/ZipDirFs/Containers/Fixtures/list_for_proxy.h \
	test/ZipDirFs/Containers/Fixtures/list_for_proxy.cpp \
	test/ZipDirFs/Containers/Fixtures/list_map_for_proxy.h \
	test/ZipDirFs/Containers/Fixtures/list_map_for_proxy.cpp \
	test/ZipDirFs/Containers/Fixtures/list_map_mock.h \
	test/ZipDirFs/Containers/Fixtures/list_map_mock.cpp \
	test/ZipDirFs/Containers/Fixtures/list_mock.h \
	test/ZipDirFs/Containers/Fixtures/list_mock.cpp \
	test/ZipDirFs/Containers/Fixtures/map_mock.h \
	test/ZipDirFs/Containers/Fixtures/map_mock.cpp \
	test/ZipDirFs/Containers/Helpers/Fixtures/ChangedMock.h \
	test/ZipDirFs/Containers/Helpers/Fixtures/ChangedMock.cpp \
	test/ZipDirFs/Containers/Helpers/Fixtures/EntryListProxyMock.h \
	test/ZipDirFs/Containers/Helpers/Fixtures/EntryListProxyMock.cpp \
	test/ZipDirFs/Containers/Helpers/Fixtures/EnumeratorMock.h \
	test/ZipDirFs/Containers/Helpers/Fixtures/EnumeratorMock.cpp \
	test/ZipDirFs/Containers/Helpers/Fixtures/FactoryMock.h \
	test/ZipDirFs/Containers/Helpers/Fixtures/FactoryMock.cpp \
	test/ZipDirFs/Utilities/Fixtures/FileSystem.h \
	test/ZipDirFs/Utilities/Fixtures/FileSystem.cpp \
	test/ZipDirFs/Zip/Fixtures/Lib.h \
	test/ZipDirFs/Zip/Fixtures/Lib.cpp \
	test/ZipDirFs/Zip/Fixtures/SimpleSpan.h \
	test/ZipDirFs/Zip/Fixtures/SimpleSpan.cpp \
	test/ZipDirFs/Zip/Fixtures/StreamBufferInitializer.h \
	test/ZipDirFs/Zip/Fixtures/StreamBufferInitializer.cpp
libgtest_local_la_CXXFLAGS = $(CXXFLAGS_COVERAGE) -D BUILD_TEST
libgtest_local_la_LIBADD = $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
libgtest_local_la_LDFLAGS = -pthread

check_PROGRAMS = gtest
gtest_SOURCES = \
	test/gtest.h \
	test/gtest.cpp \
	test/ZipDirFs/Components/NativeChanged.h \
	test/ZipDirFs/Components/NativeChanged.cpp \
	test/ZipDirFs/Components/NativeDirectoryEnumerator.h \
	test/ZipDirFs/Components/NativeDirectoryEnumerator.cpp \
	test/ZipDirFs/Components/NativeFactory.h \
	test/ZipDirFs/Components/NativeFactory.cpp \
	test/ZipDirFs/Components/ZipDirectoryEnumerator.h \
	test/ZipDirFs/Components/ZipDirectoryEnumerator.cpp \
	test/ZipDirFs/Components/ZipFactory.h \
	test/ZipDirFs/Components/ZipFactory.cpp \
	test/ZipDirFs/Components/ZipFileChanged.h \
	test/ZipDirFs/Components/ZipFileChanged.cpp \
	test/ZipDirFs/Containers/EntryGenerator.h \
	test/ZipDirFs/Containers/EntryGenerator.cpp \
	test/ZipDirFs/Containers/EntryIterator.h \
	test/ZipDirFs/Containers/EntryIterator.cpp \
	test/ZipDirFs/Containers/EntryList.h \
	test/ZipDirFs/Containers/EntryList.cpp \
	test/ZipDirFs/Containers/list_map.h \
	test/ZipDirFs/Containers/list_map.cpp \
	test/ZipDirFs/Utilities/FileSystem.h \
	test/ZipDirFs/Utilities/FileSystem.cpp \
	test/ZipDirFs/Zip/Archive.h \
	test/ZipDirFs/Zip/Archive.cpp \
	test/ZipDirFs/Zip/Entry.h \
	test/ZipDirFs/Zip/Entry.cpp \
	test/ZipDirFs/Zip/Factory.h \
	test/ZipDirFs/Zip/Factory.cpp \
	test/ZipDirFs/Zip/Lib.h \
	test/ZipDirFs/Zip/Lib.cpp \
	test/ZipDirFs/Zip/Stream.h \
	test/ZipDirFs/Zip/Stream.cpp \
	test/ZipDirFs/Zip/Base/Content.h \
	test/ZipDirFs/Zip/Base/Content.cpp
gtest_CXXFLAGS = $(CXXFLAGS_COVERAGE)
gtest_LDADD = libzipdirfs.la libgtest.la libgmock_main.la libgtest_local.la $(LDADD_COVERAGE) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
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
