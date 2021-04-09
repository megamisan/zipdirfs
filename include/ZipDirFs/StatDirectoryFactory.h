/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_STATDIRECTORYFACTORY_H
#define ZIPDIRFS_STATDIRECTORYFACTORY_H

#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <ctime>

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
		fusekit::entry* find(const char* name)
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
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &)
		{
			return 0;
		}
		inline timespec getLastUpdate()
		{
			struct timespec time;
			clock_gettime(CLOCK_REALTIME, &time);
			return time;
		}
	private:
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_STATDIRECTORYFACTORY_H
