AM_CPPFLAGS = -I $(srcdir)/../../include
AM_CXXFLAGS = -Wall -fexceptions -O2
AM_LDFLAGS = -s -Ur

ZIPDIRFS_INCLUDE = ../../include/zipdirfs

noinst_LIBRARIES = libinternal.a
libinternal_a_SOURCES = \
	EntryFactory.cpp \
	MutexLockPolicy.cpp \
	ZipEntry.cpp \
	ZipWalker.cpp \
	LinkTime.cpp \
	ZipFile.cpp \
	NotFoundException.cpp \
	stream_indent.cpp \
	ZipEntryFileInfo.cpp \
	ZipIterator.cpp \
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
