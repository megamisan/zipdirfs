/*
 * Copyright © 2021 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */

#include "ZipDirFs/Fuse/EntryProxy.h"

namespace ZipDirFs::Fuse
{
	EntryProxy& EntryProxy::operator=(std::unique_ptr<fusekit::entry>&& target)
	{
		this->target = std::move(target);
		return *this;
	}

	fusekit::entry* EntryProxy::child(const char* name) { return target->child(name); }

	int EntryProxy::stat(struct stat& stbuf) { return target->stat(stbuf); }

	int EntryProxy::access(int mode) { return target->access(mode); }

	int EntryProxy::chmod(mode_t permission) { return target->chmod(permission); }

	int EntryProxy::open(fuse_file_info& fi) { return target->open(fi); }

	int EntryProxy::release(fuse_file_info& fi) { return target->release(fi); }

	int EntryProxy::read(char* buf, size_t size, off_t offset, fuse_file_info& fi)
	{
		return target->read(buf, size, offset, fi);
	}

	int EntryProxy::write(const char* buf, size_t size, off_t offset, fuse_file_info& fi)
	{
		return target->write(buf, size, offset, fi);
	}

	int EntryProxy::opendir(fuse_file_info& fi) { return target->opendir(fi); }

	int EntryProxy::readdir(void* buf, fuse_fill_dir_t filter, off_t offset, fuse_file_info& fi)
	{
		return target->readdir(buf, filter, offset, fi);
	}

	int EntryProxy::releasedir(fuse_file_info& fi) { return target->releasedir(fi); }

	int EntryProxy::mknod(const char* name, mode_t mode, dev_t type)
	{
		return target->mknod(name, mode, type);
	}

	int EntryProxy::unlink(const char* name) { return target->unlink(name); }

	int EntryProxy::mkdir(const char* name, mode_t mode) { return target->mkdir(name, mode); }

	int EntryProxy::rmdir(const char* name) { return target->rmdir(name); }

	int EntryProxy::flush(fuse_file_info& fi) { return target->flush(fi); }

	int EntryProxy::truncate(off_t off) { return target->truncate(off); }

	int EntryProxy::utimens(const timespec times[2]) { return target->utimens(times); }

	int EntryProxy::readlink(char* buf, size_t bufsize)
	{
		return target->readlink(buf, bufsize);
	}

	int EntryProxy::symlink(const char* name, const char* target)
	{
		return this->target->symlink(name, target);
	}

	int EntryProxy::setxattr(const char* name, const char* value, size_t size, int flags)
	{
		return target->setxattr(name, value, size, flags);
	}

	int EntryProxy::getxattr(const char* name, char* value, size_t size)
	{
		return target->getxattr(name, value, size);
	}

	int EntryProxy::listxattr(char* list, size_t size) { return target->listxattr(list, size); }

	int EntryProxy::removexattr(const char* name) { return target->removexattr(name); }

} // namespace ZipDirFs::Fuse
