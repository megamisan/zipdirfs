/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_UNIONDIRECTORYFACTORY_H
#define ZIPDIRFS_UNIONDIRECTORYFACTORY_H

#include <fusekit/entry.h>
#include <fusekit/no_lock.h>
#include <ctime>

namespace ZipDirFs
{
	/**
	 * \brief Represents a join between two directory factory.
	 * This class must be used as a template parameter of \brief DirectoryNode.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class Factory1, class Factory2, class LockPolicy = fusekit::no_lock >
	class UnionDirectoryFactory : public Factory1<LockPolicy>, public Factory2<LockPolicy>
	{
	public:
		UnionDirectoryFactory() {}
		virtual ~UnionDirectoryFactory() {}
	protected:
		fusekit::entry* find(const char* name)
		{
			fusekit::entry *entry = factory1().find(name);
			if (entry == NULL) { entry = factory2().find(name); }
			return entry;
		}
		int size()
		{
			return factory1().size() + factory2.size();
		}
		int links()
		{
			return factory1().links() + factory2.links();
		}
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info &finfo)
		{
			factory1().readdir(buf, filler, offset, finfo);
			factory2().readdir(buf, filler, offset, finfo);
			return 0;
		}
		inline timespec getLastUpdate()
		{
			timespec u1 = factory1().getLastUpdate();
			timespec u2 = factory2().getLastUpdate();
			return (u1 > u2) ? u1 : u2;
		}
	private:
		inline Factory1& factory1()
		{
			return static_cast<Factory1&>(*this);
		}
		inline Factory2& factory2()
		{
			return static_cast<Factory2&>(*this);
		}
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_UNIONDIRECTORYFACTORY_H
