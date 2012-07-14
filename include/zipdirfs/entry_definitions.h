/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 *
 * This file is part of zipdirfs.
 *
 * zipdirfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zipdirfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zipdirfs.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 */
#ifndef ENTRY_DEFINITIONS_H_INCLUDED
#define ENTRY_DEFINITIONS_H_INCLUDED

#include "zipdirfs/DefaultPermission.h"
#include "zipdirfs/SystemDirectoryFactory.h"
#include "zipdirfs/SystemDirectoryTime.h"
#include "zipdirfs/DirectoryNode.h"
#include "zipdirfs/LinkFactory.h"
#include "zipdirfs/LinkTime.h"
#include "zipdirfs/LinkBuffer.h"
#include "zipdirfs/LinkNode.h"
#include "zipdirfs/ZipRootFactory.h"
#include "zipdirfs/ZipDirectoryFactory.h"
#include "zipdirfs/ZipTime.h"
#include "zipdirfs/ZipFileBuffer.h"
#include "zipdirfs/MutexLockPolicy.h"
#include "zipdirfs/DirectoryMark.h"
#include <fusekit/basic_entry.h>
#include <fusekit/basic_directory.h>
#include <fusekit/basic_file.h>
#include <fusekit/no_buffer.h>

namespace zipdirfs
{
	template <class Derived> class SystemDirectory : public DirectoryNode<SystemDirectoryFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<SystemDirectory, SystemDirectoryTime, DefaultDirectoryPermission> system_directory;

	template <class Derived> class Link : public LinkNode<LinkFactory<MutexLockPolicy>, Derived> {};
	typedef fusekit::basic_entry<LinkTime, DefaultLinkPermission, LinkBuffer, Link, S_IFLNK> wrapper_link;

	template <class Derived> class ZipRootDirectory : public DirectoryNode<ZipRootFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<ZipRootDirectory, SystemDirectoryTime, DefaultDirectoryPermission> zip_root_directory;

	template <class Derived> class ZipDirectory : public DirectoryNode<ZipDirectoryFactory<MutexLockPolicy>, Derived>, public DirectoryMark {};
	typedef fusekit::basic_directory<ZipDirectory, ZipTime, DefaultDirectoryPermission> zip_directory;

	typedef fusekit::basic_file<ZipFileBuffer, ZipTime, DefaultFilePermission> zip_file;
}

#endif // ENTRY_DEFINITIONS_H_INCLUDED
