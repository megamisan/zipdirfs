/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ENTRY_DEFINITIONS_H_INCLUDED
#define ZIPDIRFS_ENTRY_DEFINITIONS_H_INCLUDED

#include "ZipDirFs/DefaultPermission.h"
#include "ZipDirFs/SystemDirectoryFactory.h"
#include "ZipDirFs/SystemDirectoryTime.h"
#include "ZipDirFs/DirectoryNode.h"
#include "ZipDirFs/SymlinkTime.h"
#include "ZipDirFs/ZipRootFactory.h"
#include "ZipDirFs/ZipDirectoryFactory.h"
#include "ZipDirFs/ZipTime.h"
#include "ZipDirFs/ZipFileBuffer.h"
#include "ZipDirFs/MutexLockPolicy.h"
#include "ZipDirFs/DirectoryMark.h"
#include <fusekit/basic_entry.h>
#include <fusekit/basic_directory.h>
#include <fusekit/basic_file.h>
#include <fusekit/basic_symlink.h>
#include <fusekit/no_buffer.h>

namespace ZipDirFs
{
	template <class Derived> class SystemDirectory : public DirectoryNode<SystemDirectoryFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<SystemDirectory, SystemDirectoryTime, DefaultDirectoryPermission> system_directory;

	typedef fusekit::basic_symlink<SymlinkTime> wrapper_link;

	template <class Derived> class ZipRootDirectory : public DirectoryNode<ZipRootFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<ZipRootDirectory, SystemDirectoryTime, DefaultDirectoryPermission> zip_root_directory;

	template <class Derived> class ZipDirectory : public DirectoryNode<ZipDirectoryFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<ZipDirectory, ZipTime, DefaultDirectoryPermission> zip_directory;

	typedef fusekit::basic_file<ZipFileBuffer, ZipTime, DefaultFilePermission> zip_file;
} // namespace ZipDirFs

#endif // ZIPDIRFS_ENTRY_DEFINITIONS_H_INCLUDED
