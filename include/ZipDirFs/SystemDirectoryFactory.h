/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_SYSTEMDIRECTORYFACTORY_H
#define ZIPDIRFS_SYSTEMDIRECTORYFACTORY_H

#include "ZipDirFs/NameSearchTree.h"
#include "ZipDirFs/EntryFactory.h"
#include "ZipDirFs/DirectoryMark.h"
#include "ZipDirFs/utils.h"
#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <cstddef>
#include <iostream>

namespace ZipDirFs
{
	/**
	 * \brief Returns the canonicalized absolute pathname
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	bool realpath(const std::string& path, std::string& resolved_path);
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
		SystemDirectoryFactory() : entries(deleteEntry), folderCount(0), lastUpdate( {0}) {};
		/** Default destructor */
		virtual ~SystemDirectoryFactory() {};
		/**
		 * \brief Defines the path to the underlying system directory.
		 * The path is canonicalized.
		 * \param path The target path.
		 */
		bool setRealPath(const std::string& path)
		{
			if (realPath.empty())
			{
				return realpath(path, realPath);
			}
			return true;
		}
		/**
		 * \brief Retrieves the path to the underlying system directory.
		 * \return The underlying path.
		 */
		const std::string& getRealPath() const
		{
			return this->realPath;
		}
		fusekit::entry* find(const char* name)
		{
			this->checkSystem();
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
			this->checkSystem();
			lock guard(*this);
			return entries.size();
		}
		int links()
		{
			this->checkSystem();
			lock guard(*this);
			return folderCount;
		}
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			this->checkSystem();
			lock guard(*this);

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				filler(buf, it->c_str(), NULL, offset);
			}

			return 0;
		}
		inline timespec getLastUpdate() const
		{
			return this->lastUpdate;
		}
	protected:
	private:
		tree entries;
		int folderCount;
		std::string realPath;
		struct timespec lastUpdate;
		static void deleteEntry(fusekit::entry* e)
		{
			if (e != NULL) { delete e; }
		}
		/**
		 * \brief Checks the underlying system directory for update.
		 */
		void checkSystem()
		{
			struct ::stat pathinfo;
			int res = ::stat(realPath.c_str(), &pathinfo);

			if ((res != 0))
			{
				::perror("SystemDirectoryFactory::checkSystem: stat");
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
			lock guard(*this);
			tree toRemove;
			fusekit::entry* nullEntry = NULL;

			for (tree::iterator it = entries.begin(); it != entries.end(); it++)
			{
				toRemove.add(it->c_str(), nullEntry);
			}

			::DIR* folder = NULL;
			folder = ::opendir(this->realPath.c_str());

			if (folder == NULL)
			{
				::perror("SystemDirectoryFactory::updateEntries: opendir");
				std::cerr << "Real path " << realPath << std::endl;
			}
			else
			{
				::dirent* dirEntry = NULL;
				fusekit::entry* holder = NULL;

				while ((dirEntry = ::readdir(folder)))
				{
					if ((::strcmp(dirEntry->d_name, ".") != 0) && (::strcmp(dirEntry->d_name, "..") != 0))     // Skip special entries.
					{
						toRemove.remove(dirEntry->d_name);

						// if (entries.isset(dirEntry->d_name))
						// {
						// 	// TODO: Check whether the entry type has changed.
						// }
						if (!entries.isset(dirEntry->d_name))
						{
							entries.add(dirEntry->d_name, holder = EntryFactory::newEntry(dirEntry, this->realPath));
							if (dynamic_cast<DirectoryMark*>(holder) != NULL) { folderCount++; }
						}
					}
				}
				::closedir(folder);
			}

			for (tree::iterator it = toRemove.begin(); it != toRemove.end(); it++)
			{
				if (dynamic_cast<DirectoryMark*>(entries.get(it->c_str())) != NULL) { folderCount--; }
				entries.remove(it->c_str());
			}
		}
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_SYSTEMDIRECTORYFACTORY_H
