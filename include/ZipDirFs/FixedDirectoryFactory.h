/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 */
#ifndef FIXEDDIRECTORYFACTORY_H
#define FIXEDDIRECTORYFACTORY_H

#include "ZipDirFs/NameSearchTree.h"
#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <time.h>

namespace ZipDirFs
{
	/**
	 * \brief Represents a directory factory filled by external code.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class LockPolicy = fusekit::no_lock >
	class FixedDirectoryFactory
	{
		typedef typename SystemDirectoryFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		FixedDirectoryFactory() : entries(deleteEntry), folderCount(0), lastUpdate(time(NULL)) {}
		virtual ~FixedDirectoryFactory() {}
		/**
		 * \brief Adds the specified entry to the directory.
		 * If an entry is already defined with the specified name, it fails.
		 * \param name The name of the entry to add.
		 * \param entry The \ref fusekit::entry instance defining the entry.
		 * \return `true` on success. `false` on failure.
		 */
		bool addEntry(const char* name, fusekit::entry* entry)
		{

		}
		/**
		 * \brief Removes the specified entry from the directory.
		 * If the entry is not found, it fails.
		 * \param name The name of the entry to remove.
		 * \return `true` on success. `false` on failure.
		 */
		bool removeEntry(const char* name)
		{

		}
	protected:
		fusekit::entry* find(const char* name)
		{
			lock guard(*this);

			try
			{
				return entries.get(name);
			}
			catch (NotFoundException)
			{
				return NULL;
			}
		}
		int size()
		{
			lock guard(*this);
			return entries.size();
		}
		int links()
		{
			lock guard(*this);
			return folderCount;
		}
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			lock guard(*this);

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler(buf, it->c_str(), NULL, offset);
			}

			return 0;
		}
		inline timespec getLastUpdate()
		{
			return lastUpdate;
		}
	private:
		tree entries;
		int folderCount;
		struct timespec lastUpdate;
		static void deleteEntry(fusekit::entry* e)
		{
			if (e != NULL) { delete e; }
		}
	};
}

#endif // FIXEDDIRECTORYFACTORY_H
