/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_DIRECTORYNODE_H
#define ZIPDIRFS_DIRECTORYNODE_H

#include <fusekit/entry.h>

namespace ZipDirFs
{
	/**
	 * \brief Represents a node for a directory.
	 * This class must be used as the %Node template parameter to the \ref fusekit::basic_entry template class.
	 * However, to do so, a compatible factory class must also be provided by using an intermediate type
	 * specifying only the %Factory template parameter, like
	 * `template <class Derived> class MyNode : DirectoryNode<MyFactory, Derived> {}`.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template <class Factory, class Derived>
	class DirectoryNode : public Factory
	{
	public:
		/** Default constructor */
		DirectoryNode() {};
		/** Default destructor */
		virtual ~DirectoryNode() {};
		fusekit::entry* find(const char* name)
		{
			return factory().find(name);
		}
		int target(char*, size_t)
		{
			return -EINVAL;
		}
		int links()
		{
			return factory().links() + 2;
		}
		int opendir(::fuse_file_info& finfo)
		{
			return 0;
		}
		int readdir(void* buf, ::fuse_fill_dir_t filler, ::off_t offset, ::fuse_file_info& finfo)
		{
			filler(buf, ".", NULL, offset);
			filler(buf, "..", NULL, offset);
			factory().readdir(buf, filler, offset, finfo);
			return 0;
		}
		int releasedir(::fuse_file_info& finfo)
		{
			return 0;
		}
		int mknod(const char* name, ::mode_t mode, ::dev_t type)
		{
			return -EACCES;
		}
		int unlink(const char* name)
		{
			return -EACCES;
		}
		int mkdir(const char* name, ::mode_t mode)
		{
			return -EACCES;
		}
		int rmdir(const char* name)
		{
			return -EACCES;
		}
		int symlink(const char* name, const char* target)
		{
			return -EPERM;
		}
	protected:
	private:
		inline Factory& factory()
		{
			return static_cast<Factory&>(*this);
		}
	};
} // namespace ZipDirFs

#endif // ZIPDIRFS_DIRECTORYNODE_H
