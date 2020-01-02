SUBDIRS = ZipDirFs
AM_CPPFLAGS = -I $(top_srcdir)/include
AM_CXXFLAGS = -Wall -fexceptions
LDADD = ZipDirFs/libinternal.a

ROOT_INCLUDE = $(top_srcdir)/include
ZIPDIRFS_INCLUDE = $(top_srcdir)/include/ZipDirFs

bin_PROGRAMS = zipdirfs
zipdirfs_SOURCES = \
	Main.cpp \
	Options.cpp \
	$(ROOT_INCLUDE)/Main.h \
	$(ROOT_INCLUDE)/Options.h \
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
	$(ZIPDIRFS_INCLUDE)/EntryFactory.h \
	$(ZIPDIRFS_INCLUDE)/UnionDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchWalker.h \
	$(ZIPDIRFS_INCLUDE)/ZipEntryFileInfo.h \
	$(ZIPDIRFS_INCLUDE)/DirectoryMark.h \
	$(ZIPDIRFS_INCLUDE)/SymlinkTime.h \
	$(ZIPDIRFS_INCLUDE)/DirectoryNode.h \
	$(ZIPDIRFS_INCLUDE)/entry_definitions.h \
	$(ZIPDIRFS_INCLUDE)/ZipRootFactory.h \
	$(ZIPDIRFS_INCLUDE)/SystemDirectoryFactory.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchTreeNodes.h \
	$(ZIPDIRFS_INCLUDE)/ZipFileBuffer.h \
	$(ZIPDIRFS_INCLUDE)/MutexLockPolicy.h \
	$(ZIPDIRFS_INCLUDE)/ZipFile.h \
	$(ZIPDIRFS_INCLUDE)/NameSearchTree.h \
	$(ZIPDIRFS_INCLUDE)/utils.h
