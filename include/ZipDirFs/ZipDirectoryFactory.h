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
#ifndef ZIPDIRECTORYFACTORY_H
#define ZIPDIRECTORYFACTORY_H

#include "ZipDirFs/ZipWalker.h"
#include "ZipDirFs/ZipFile.h"
#include "ZipDirFs/DirectoryMark.h"
#include <fusekit/no_lock.h>
#include <fusekit/entry.h>
#include <string>

namespace ZipDirFs
{
	/**
	 * \brief Represents a directory in a Zip file.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class LockingPolicy = fusekit::no_lock >
	class ZipDirectoryFactory : public LockingPolicy
	{
		typedef typename ZipDirectoryFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		ZipDirectoryFactory() : entries(deleteEntry), folderCount(0), source(NULL), mtime( {0}) {}
		virtual ~ZipDirectoryFactory() {}
		fusekit::entry* find(const char* name)
		{
			this->checkFile();
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
			this->checkFile();
			lock guard(*this);
			return entries.size();
		}
		int links()
		{
			this->checkFile();
			lock guard(*this);
			return folderCount;
		}
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			this->checkFile();
			lock guard(*this);

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler(buf, it->c_str(), NULL, offset);
			}

			return 0;
		}
		inline timespec getMTime()
		{
			return this->mtime;
		}
		friend class ZipWalker;
	protected:
	private:
		tree entries;
		int folderCount;
		std::string relativePath;
		ZipFile* source;
		struct timespec mtime;
		/**
		 * \brief Defines the directory info.
		 * \param source The ZipFile in which this directory is found.
		 * \param relativePath The path of the directory in the source.
		 * \param mtime The modification time of the directory.
		 */
		void setDirectoryInfo(ZipFile* source, std::string relativePath, ::time_t mtime)
		{
			this->source = source;
			this->relativePath = relativePath;
			this->mtime = { mtime, 0 };
		}
		static void deleteEntry(fusekit::entry* e)
		{
			if (e != NULL) { delete e; }
		}
		/**
		 * \brief Loads inner entries from the source.
		 */
		void checkFile()
		{
			lock guard(*this);

			if (this->source != NULL)
			{
				ZipWalker it(this->source, this->relativePath, false);
				ZipWalker end(this->source, this->relativePath, true);
				for (; it != end; it++)
				{
					fusekit::entry *entry = it->second;
					if (dynamic_cast<DirectoryMark*>(entry) != NULL) { folderCount++; }
					entries.add(it->first.c_str(), entry);
				}
				this->source = NULL;
			}
		}
	};
}

#endif // ZIPDIRECTORYFACTORY_H
