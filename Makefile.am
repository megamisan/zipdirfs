AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(top_builddir)/include
AM_CXXFLAGS = -Wall -fexceptions -O2
AM_LDFLAGS = -s 

ROOT_INCLUDE = $(top_builddir)/include
ROOT_SRC = $(top_builddir)/src
ZIPDIRFS_INCLUDE = $(top_builddir)/include/zipdirfs
ZIPDIRFS_SRC = $(top_builddir)/src/zipdirfs
TEST_INCLUDE = $(top_builddir)/include/test
TEST_SRC = $(top_builddir)/src/test

SHARED_FILES = \
	$(ZIPDIRFS_SRC)/EntryFactory.cpp \
	$(ZIPDIRFS_SRC)/MutexLockPolicy.cpp \
	$(ZIPDIRFS_SRC)/ZipEntry.cpp \
	$(ZIPDIRFS_SRC)/ZipWalker.cpp \
	$(ZIPDIRFS_SRC)/LinkTime.cpp \
	$(ZIPDIRFS_SRC)/ZipFile.cpp \
	$(ZIPDIRFS_SRC)/NotFoundException.cpp \
	$(ZIPDIRFS_SRC)/stream_indent.cpp \
	$(ZIPDIRFS_SRC)/ZipEntryFileInfo.cpp \
	$(ZIPDIRFS_SRC)/ZipIterator.cpp \
	$(ZIPDIRFS_INCLUDE)/StatDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/ZipDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/DefaultPermission.h \
	$(ZIPDIRFS_INCLUDE)/ZipEntry.h \
	$(ZIPDIRFS_INCLUDE)/ZipIterator.h \
	$(ZIPDIRFS_INCLUDE)/stream_indent.h \
	$(ZIPDIRFS_INCLUDE)/ZipWalker.h \
	$(ZIPDIRFS_INCLUDE)/SystemDirectoryTime.h \
	$(ZIPDIRFS_INCLUDE)/ZipTime.h \
	$(ZIPDIRFS_INCLUDE)/NotFoundException.h \
	$(ZIPDIRFS_INCLUDE)/FixedDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/LinkFactory.h \
	$(ZIPDIRFS_INCLUDE)/EntryFactory.h \
	$(ZIPDIRFS_INCLUDE)/UnionDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchWalker.h \
	$(ZIPDIRFS_INCLUDE)/ZipEntryFileInfo.h \
	$(ZIPDIRFS_INCLUDE)/LinkNode.h \
	$(ZIPDIRFS_INCLUDE)/DirectoryMark.h \
	$(ZIPDIRFS_INCLUDE)/LinkTime.h \
	$(ZIPDIRFS_INCLUDE)/DirectoryNode.h \
	$(ZIPDIRFS_INCLUDE)/entry_definitions.h \
	$(ZIPDIRFS_INCLUDE)/ZipRootFactory.h \
	$(ZIPDIRFS_INCLUDE)/LinkBuffer.h \
	$(ZIPDIRFS_INCLUDE)/SystemDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchTreeNodes.h \
	$(ZIPDIRFS_INCLUDE)/ZipFileBuffer.h \
	$(ZIPDIRFS_INCLUDE)/MutexLockPolicy.h \
	$(ZIPDIRFS_INCLUDE)/ZipFile.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchTree.h

bin_PROGRAMS = zipdirfs zipdirfs-test
zipdirfs_SOURCES = \
	$(ROOT_SRC)/Main.cpp \
	$(ROOT_SRC)/Options.cpp \
	$(ROOT_INCLUDE)/Main.h \
	$(ROOT_INCLUDE)/Options.h \
	$(SHARED_FILES)

zipdirfs_test_SOURCES = maintest.cpp \
	$(SHARED_FILES)

