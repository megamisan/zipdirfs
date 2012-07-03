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
#ifndef ZIPDIRECTORYFACTORY_H
#define ZIPDIRECTORYFACTORY_H

#include "zipdirfs/ZipWalker.h"
#include "zipdirfs/ZipFile.h"
#include <fusekit/no_lock.h>
#include <fusekit/entry.h>
#include <string>

namespace zipdirfs
{
	template < class LockingPolicy = fusekit::no_lock >
	class ZipDirectoryFactory : public LockingPolicy
	{
		typedef typename ZipDirectoryFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		ZipDirectoryFactory() : entries (deleteEntry), source (NULL), mtime (0) {}
		virtual ~ZipDirectoryFactory() {}
		fusekit::entry* find (const char* name)
		{
			this->checkFile();
			lock guard (*this);

			try
			{
				return entries.get (name);
			}
			catch (NotFoundException)
			{
				return NULL;
			}
		}
		int size()
		{
			this->checkFile();    //TODO: Count only directories. Internal value.
			lock guard (*this);
			return entries.size();
		}
		int readdir (void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			this->checkFile();
			lock guard (*this);

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler (buf, it->c_str(), NULL, offset);
			}

			return 0;
		}
		inline ::time_t getMTime()
		{
			return this->mtime;
		}
		friend class ZipWalker;
	protected:
	private:
		tree entries;
		std::string relativePath;
		ZipFile* source;
		::time_t mtime;
		void setDirectoryInfo (ZipFile* source, std::string relativePath, ::time_t mtime)
		{
			this->source = source;
			this->relativePath = relativePath;
			this->mtime = mtime;
		}
		static void deleteEntry (fusekit::entry* e)
		{
			if (e != NULL) delete e;
		}
		void checkFile()
		{
			lock guard (*this);

			if (source != NULL)
			{
				// DO THINGS
				source = NULL;
			}
		}
	};
}

#endif // ZIPDIRECTORYFACTORY_H
