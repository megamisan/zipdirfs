/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_DIRECTORYNODE_H
#define ZIPDIRFS_FUSE_DIRECTORYNODE_H

#include "ZipDirFs/Containers/EntryGenerator.h"
#include <fuse.h>

namespace ZipDirFs::Fuse
{
	using ::ZipDirFs::Containers::EntryGenerator;

	template <class Derived, class Generator = EntryGenerator>
	struct DirectoryNode
	{
		fusekit::entry* find(const char* name)
		{
			// TODO: Sync with updates / First fetch
			return proxy()->find(name);
		}
		nlink_t links()
		{
			// TODO: Precompute valid when updating
			return 2;
		}
		int opendir(fuse_file_info& fi) { return 0; }
		int readdir(void* buf, fuse_fill_dir_t filler, off_t offset, fuse_file_info& fi)
		{
			auto iterator =
				(offset == 0 || offset == 1) ? generator().begin() : generator().remove(offset);
			bool ok = true;
			if (offset == 0)
			{
				offset = 1;
				ok = !filler(buf, ".", nullptr, offset);
			}
			if (ok && offset == 1)
			{
				offset = 2;
				ok = !filler(buf, "..", nullptr, offset);
			}
			if (ok && offset == 2)
			{
				while (offset == 2 || offset == 1)
				{
					timespec now;
					clock_gettime(CLOCK_REALTIME, &now);
					offset = now.tv_nsec + now.tv_sec;
				}
			}
			auto end = generator().end();
			while (ok && !(iterator == end))
			{
				ok = !filler(buf, (*iterator).c_str(), nullptr, offset);
				if (ok)
				{
					++iterator;
				}
			}
			if (!ok)
			{
				generator().add(std::move(iterator), offset);
			}
			return 0;
		}
		int releasedir(fuse_file_info& fi) { return 0; }
		int mknod(const char* name, mode_t mode, dev_t type) { return -EACCES; }
		int unlink(const char* name) { return -EACCES; }
		int mkdir(const char* name, mode_t mode) { return -EACCES; }
		int rmdir(const char* name) { return -EACCES; }
		int symlink(const char* name, const char* target) { return -EPERM; }

		virtual Generator& generator() = 0;
		virtual typename Generator::proxy_ptr& proxy() = 0;
	};

	template <class Derived>
	struct DefaultDirectoryNode : public DirectoryNode<Derived>
	{
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_DIRECTORYNODE_H
