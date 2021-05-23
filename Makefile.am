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

noinst_LTLIBRARIES = libzipdirfs.la
libzipdirfs_la_SOURCES = \
	src/ZipDirFs/Components/ChangedProxy.cpp \
	src/ZipDirFs/Components/ChangedStart.cpp \
	src/ZipDirFs/Components/EntryListProxyIncremental.cpp \
	src/ZipDirFs/Components/EntryListProxyProxy.cpp \
	src/ZipDirFs/Components/NativeChanged.cpp \
	src/ZipDirFs/Components/NativeDirectoryEnumerator.cpp \
	src/ZipDirFs/Components/ZipDirectoryEnumerator.cpp \
	src/ZipDirFs/Components/ZipFileChanged.cpp \
	src/ZipDirFs/Containers/EntryGenerator.cpp \
	src/ZipDirFs/Containers/EntryIterator.cpp \
	src/ZipDirFs/Fuse/EntryProxy.cpp \
	src/ZipDirFs/Fuse/NativeDirectory.cpp \
	src/ZipDirFs/Fuse/NativeSymlink.cpp \
	src/ZipDirFs/Fuse/ZipDirectory.cpp \
	src/ZipDirFs/Fuse/ZipFile.cpp \
	src/ZipDirFs/Fuse/ZipRootDirectory.cpp \
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
	include/ZipDirFs/Components/ChangedProxy.h \
	include/ZipDirFs/Components/ChangedStart.h \
	include/ZipDirFs/Components/EntryListProxyIncremental.h \
	include/ZipDirFs/Components/EntryListProxyProxy.h \
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
	include/ZipDirFs/Fuse/DirectoryNode.h \
	include/ZipDirFs/Fuse/EntryProxy.h \
	include/ZipDirFs/Fuse/NativeDirectory.h \
	include/ZipDirFs/Fuse/NativeSymlink.h \
	include/ZipDirFs/Fuse/NativeTimePolicy.h \
	include/ZipDirFs/Fuse/ZipDirectory.h \
	include/ZipDirFs/Fuse/ZipFile.h \
	include/ZipDirFs/Fuse/ZipFileBuffer.h \
	include/ZipDirFs/Fuse/ZipRootDirectory.h \
	include/ZipDirFs/Fuse/ZipTimePolicy.h \
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
	test/ZipDirFs/Fuse/Fixtures/EmptyEnumerator.h \
	test/ZipDirFs/Fuse/Fixtures/EmptyEnumerator.cpp \
	test/ZipDirFs/Fuse/Fixtures/FunctionalEnumerator.h \
	test/ZipDirFs/Fuse/Fixtures/FunctionalEnumerator.cpp \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemon.h \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemon.cpp \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemonFork.h \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemonFork.cpp \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemonThread.h \
	test/ZipDirFs/Fuse/Fixtures/FuseDaemonThread.cpp \
	test/ZipDirFs/Fuse/Fixtures/NullFactory.h \
	test/ZipDirFs/Fuse/Fixtures/NullFactory.cpp \
	test/ZipDirFs/Fuse/Fixtures/OneChanged.h \
	test/ZipDirFs/Fuse/Fixtures/OneChanged.cpp \
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

check_PROGRAMS = gtest gtest_helper
gtest_SOURCES = \
	test/gtest.h \
	test/gtest.cpp \
	test/ZipDirFs/Integration.h \
	test/ZipDirFs/Integration.cpp \
	test/ZipDirFs/Components/ChangedProxy.h \
	test/ZipDirFs/Components/ChangedProxy.cpp \
	test/ZipDirFs/Components/ChangedStart.h \
	test/ZipDirFs/Components/ChangedStart.cpp \
	test/ZipDirFs/Components/EntryListProxyIncremental.h \
	test/ZipDirFs/Components/EntryListProxyIncremental.cpp \
	test/ZipDirFs/Components/EntryListProxyProxy.h \
	test/ZipDirFs/Components/EntryListProxyProxy.cpp \
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
	test/ZipDirFs/Fuse/DirectoryNode.h \
	test/ZipDirFs/Fuse/DirectoryNode.cpp \
	test/ZipDirFs/Fuse/EntryProxy.h \
	test/ZipDirFs/Fuse/EntryProxy.cpp \
	test/ZipDirFs/Fuse/NativeTimePolicy.h \
	test/ZipDirFs/Fuse/NativeTimePolicy.cpp \
	test/ZipDirFs/Fuse/ZipFileBuffer.h \
	test/ZipDirFs/Fuse/ZipFileBuffer.cpp \
	test/ZipDirFs/Fuse/ZipTimePolicy.h \
	test/ZipDirFs/Fuse/ZipTimePolicy.cpp \
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

gtest_helper_SOURCES = \
	test/gtest_helper.h \
	test/gtest_helper.cpp \
	test/ZipDirFs/Worker/Explore.cpp \
	test/ZipDirFs/Fuse/Worker/DirectoryNodeList.cpp \
	test/ZipDirFs/Fuse/Worker/FileRead.cpp \
	test/ZipDirFs/Fuse/Worker/FileStat.cpp
gtest_helper_LDADD = $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)
gtest_helper_LDFLAGS = -pthread

gtest-coverage: gtest gtest_helper
	bin/build-coverage.sh gtest "$(abs_top_builddir)"

gtest-coverage-case: gtest gtest_helper
	bin/build-case-coverage.sh gtest "$(abs_top_builddir)"

gtest-coverage-all: gtest gtest_helper
	bin/build-detailed-coverage.sh gtest "$(abs_top_builddir)"

clean-coverage:
	find coverage/ -name '*.info' -print0 | xargs -0r rm

TESTS = gtest
endif
