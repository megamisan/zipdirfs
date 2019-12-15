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
#ifndef STATDIRECTORYFACTORY_H
#define STATDIRECTORYFACTORY_H

#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <time.h>

namespace ZipDirFs
{
	/**
	 * \brief Represents a directory factory filled by statistic providers.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class LockPolicy = fusekit::no_lock >
	class StatDirectoryFactory
	{
		typedef typename SystemDirectoryFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		StatDirectoryFactory() {}
		virtual ~StatDirectoryFactory() {}
	protected:
		fusekit::entry* find (const char* name)
		{
			return NULL;
		}
		int size()
		{
			return 0;
		}
		int links()
		{
			return 0;
		}
		int readdir (void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			return 0;
		}
		inline ::time_t getLastUpdate()
		{
			return time(NULL);
		}
	private:
	};
}

#endif // STATDIRECTORYFACTORY_H
