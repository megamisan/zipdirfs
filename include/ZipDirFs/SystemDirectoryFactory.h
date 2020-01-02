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
#ifndef SYSTEMDIRECTORYFACTORY_H
#define SYSTEMDIRECTORYFACTORY_H

#include "ZipDirFs/NameSearchTree.h"
#include "ZipDirFs/EntryFactory.h"
#include "ZipDirFs/DirectoryMark.h"
#include "ZipDirFs/utils.h"
#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>

namespace ZipDirFs
{
	/**
	 * \brief Represents a directory factory mirroring a system directory.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class LockingPolicy = fusekit::no_lock >
	class SystemDirectoryFactory : public LockingPolicy
	{
		typedef typename SystemDirectoryFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		/** Default constructor */
		SystemDirectoryFactory() : entries (deleteEntry), folderCount(0), lastUpdate ({0}) {};
		/** Default destructor */
		virtual ~SystemDirectoryFactory() {};
		/**
		 * \brief Defines the path to the underlying system directory.
		 * The path is canonicalized.
		 * \param path The target path.
		 */
		void setRealPath (const char* path)
		{
			if (!realPath.empty() )
			{
				return;
			}

			char* absolute = ::realpath (path, NULL);

			if (absolute != NULL)
			{
				realPath = absolute;
				::free (absolute);
			}
		}
		/**
		 * \brief Retrieves the path to the underlying system directory.
		 * \return The underlying path.
		 */
		std::string getRealPath() const
		{
			return this->realPath;
		}
		fusekit::entry* find (const char* name)
		{
			this->checkSystem();
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
			this->checkSystem();
			lock guard (*this);
			return entries.size();
		}
		int links()
		{
			this->checkSystem();
			lock guard (*this);
			return folderCount;
		}
		int readdir (void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			this->checkSystem();
			lock guard (*this);

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler (buf, it->c_str(), NULL, offset);
			}

			return 0;
		}
		inline timespec getLastUpdate()
		{
			return this->lastUpdate;
		}
	protected:
	private:
		tree entries;
		int folderCount;
		std::string realPath;
		struct timespec lastUpdate;
		static void deleteEntry (fusekit::entry* e)
		{
			if (e != NULL) delete e;
		}
		/**
		 * \brief Checks the underlying system directory for update.
		 */
		void checkSystem()
		{
			struct ::stat pathinfo;
			int res = ::stat (realPath.c_str(), &pathinfo);

			if ( (res != 0) )
			{
				::perror ("SystemDirectoryFactory::checkSystem: stat");
				std::cerr << "Real path " << realPath << std::endl;
				return;
			}

			if (!equals(this->lastUpdate, pathinfo.st_mtim))
			{
				this->updateEntries();
				this->lastUpdate = pathinfo.st_mtim;
			}
		}
		/**
		 * \brief Updates the image of the underlying system directory.
		 * \bug Recreating a node in the source file system without accessing the mirorred version
		 * will not update the type of associated entry.
		 */
		void updateEntries()
		{
			lock guard (*this);
			tree toRemove;
			fusekit::entry* nullEntry = NULL;

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				toRemove.add (it->c_str(), nullEntry);
			}

			::DIR* folder = NULL;
			folder = ::opendir (this->realPath.c_str() );

			if (folder == NULL)
			{
				::perror ("SystemDirectoryFactory::updateEntries: opendir");
				std::cerr << "Real path " << realPath << std::endl;
			}
			else
			{
				::dirent* dirEntry = NULL;
				fusekit::entry* holder = NULL;

				while ( (dirEntry = ::readdir (folder)) )
				{
					if ( (::strcmp (dirEntry->d_name, ".") != 0) && (::strcmp (dirEntry->d_name, "..") != 0) ) // Skip special entries.
					{
						toRemove.remove (dirEntry->d_name);

						// if (entries.isset(dirEntry->d_name))
						// {
						// 	// TODO: Check whether the entry type has changed.
						// }
						if (!entries.isset (dirEntry->d_name) )
						{
							entries.add (dirEntry->d_name, holder = EntryFactory::newEntry (dirEntry, this->realPath) );
							if (dynamic_cast<DirectoryMark*>(holder) != NULL) folderCount++;
						}
					}
				}
				::closedir (folder);
			}

			for (tree::iterator it = toRemove.begin(); it != toRemove.end(); it++)
			{
				if (dynamic_cast<DirectoryMark*>(entries.get(it->c_str())) != NULL) folderCount--;
				entries.remove (it->c_str() );
			}
		}
	};
}

#endif // SYSTEMDIRECTORYFACTORY_H
