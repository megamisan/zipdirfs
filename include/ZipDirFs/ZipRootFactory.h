/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_ZIPROOTACTORY_H
#define ZIPDIRFS_ZIPROOTFACTORY_H

#include "ZipDirFs/NameSearchTree.h"
#include "ZipDirFs/ZipWalker.h"
#include "ZipDirFs/ZipFile.h"
#include "ZipDirFs/DirectoryMark.h"
#include "ZipDirFs/utils.h"
#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <ctime>

namespace ZipDirFs
{
	template < class LockingPolicy = fusekit::no_lock >
	class ZipRootFactory : public LockingPolicy
	{
		typedef typename ZipRootFactory<LockingPolicy>::lock lock;
		typedef NameSearchTree<fusekit::entry*, true> tree;
	public:
		ZipRootFactory() : entries(deleteEntry), folderCount(0), zipFile(NULL), lastUpdate( {0}) {}
		virtual ~ZipRootFactory() { if (this->zipFile != NULL) { delete this->zipFile; } }
		void setZipFile(const char* path)
		{
			if (this->zipFile)
			{
				return;
			}

			char* absolute = ::realpath(path, NULL);

			if (absolute != NULL)
			{
				this->zipFile = new ZipFile(absolute);
				free(absolute);
			}
		}
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
		inline timespec getLastUpdate()
		{
			return this->lastUpdate;
		}
	protected:
	private:
		tree entries;
		int folderCount;
		ZipFile* zipFile;
		struct timespec lastUpdate;
		static void deleteEntry(fusekit::entry* e)
		{
			if (e != NULL) { delete e; }
		}
		void checkFile()
		{
			struct ::stat pathinfo;
			int res = ::stat(zipFile->getFilePath().c_str(), &pathinfo);

			if ((res != 0))
			{
				::perror("ZipRootFactory::checkFile: stat");
				std::cerr << "Real path " << zipFile->getFilePath() << std::endl;
				return;
			}

			if (!equals(this->lastUpdate, pathinfo.st_mtim))
			{
				this->updateEntries();
				this->lastUpdate = pathinfo.st_mtim;
			}
		}
		void updateEntries()
		{
			lock guard(*this);
			folderCount = 0;
			entries.clear();
			ZipWalker it(this->zipFile, "", false);
			ZipWalker end(this->zipFile, "", true);

			for (; it != end; it++)
			{
				fusekit::entry* entry = it->second;
				if (dynamic_cast<DirectoryMark*>(entry) != NULL) { folderCount++; }
				entries.add(it->first.c_str(), entry);
			}

			std::string name;
			while (entries.size() == 1)
			{
				name.append(*entries.begin());
				struct ::stat bypassStat;
				entries.get(name.c_str())->stat(bypassStat);

				if (!S_ISDIR(bypassStat.st_mode)) { break; }
				name.append("/");
				it = ZipWalker(this->zipFile, name.c_str(), false);
				end = ZipWalker(this->zipFile, name.c_str(), true);
				entries.clear();

				for (; it != end; it++)
				{
					fusekit::entry* entry = it->second;
					if (dynamic_cast<DirectoryMark*>(entry) != NULL) { folderCount++; }
					entries.add(it->first.c_str(), entry);
				}
			}
		}
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_ZIPROOTFACTORY_H
