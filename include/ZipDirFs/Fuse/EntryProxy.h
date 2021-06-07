/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_FUSE_PROXYDIRECTORY_H
#define ZIPDIRFS_FUSE_PROXYDIRECTORY_H

#include <fusekit/entry.h>
#include <memory>

namespace ZipDirFs::Fuse
{
	/**
	 * @brief A fusekit entry which proxy calls to another and can be changed
	 * @remarks Mainly used to bootstrap the daemon.
	 * @authors Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	class EntryProxy : public fusekit::entry
	{
	public:
		EntryProxy() = default;
		EntryProxy& operator=(std::unique_ptr<fusekit::entry>&& target);
		fusekit::entry* child(const char*);
		int stat(struct stat&);
		int access(int);
		int chmod(mode_t);
		int open(fuse_file_info&);
		int release(fuse_file_info&);
		int read(char*, size_t, off_t, fuse_file_info&);
		int write(const char*, size_t, off_t, fuse_file_info&);
		int opendir(fuse_file_info&);
		int readdir(void*, fuse_fill_dir_t, off_t, fuse_file_info&);
		int releasedir(fuse_file_info&);
		int mknod(const char*, mode_t, dev_t);
		int unlink(const char*);
		int mkdir(const char*, mode_t);
		int rmdir(const char*);
		int flush(fuse_file_info&);
		int truncate(off_t);
		int utimens(const timespec[2]);
		int readlink(char*, size_t);
		int symlink(const char*, const char*);
		int setxattr(const char*, const char*, size_t, int);
		int getxattr(const char*, char*, size_t);
		int listxattr(char*, size_t);
		int removexattr(const char*);

	private:
		std::unique_ptr<fusekit::entry> target;
	};
} // namespace ZipDirFs::Fuse

#endif // ZIPDIRFS_FUSE_PROXYDIRECTORY_H
